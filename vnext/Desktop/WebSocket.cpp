// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#pragma warning(push)
#pragma warning(disable : 4996) // std::copy::_Unchecked_iterators::_Deprecate

#include "WebSocket.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include "Unicode.h"

using namespace boost::archive::iterators;
using namespace boost::asio;
using namespace boost::beast;

using boost::asio::ip::basic_resolver_iterator;
using boost::asio::ip::tcp;
using boost::asio::async_initiate;

using std::function;
using std::make_unique;
using std::size_t;
using std::string;
using std::unique_ptr;

using boostecr = boost::system::error_code const &;

namespace Microsoft::React {

#pragma region BaseWebSocket members

template <typename SocketLayer, typename Stream, typename Resolver>
BaseWebSocket<SocketLayer, Stream, Resolver>::BaseWebSocket(Url &&url) : m_url{std::move(url)} {}

template <typename SocketLayer, typename Stream, typename Resolver>
BaseWebSocket<SocketLayer, Stream, Resolver>::~BaseWebSocket() {
  if (!m_context.stopped()) {
    if (!m_closeRequested)
      Close(CloseCode::GoingAway, "Terminating instance");
    else if (!m_closeInProgress)
      PerformClose();
  }
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Handshake(const IWebSocket::Options &options) {

  //TODO: 
  //m_stream->set_option(websocket::stream_base::timeout::suggested(role_type::client));
  m_stream->set_option(websocket::stream_base::decorator(
    [options = std::move(options)](websocket::request_type& req)
    {
      // Collect headers
      for (const auto& header : options) {
        req.insert(Microsoft::Common::Unicode::Utf16ToUtf8(header.first), header.second);
      }
    }
  ));

  m_stream->async_handshake(
    m_url.host,
    m_url.Target(),
    [this](boostecr ec)
    {
      if (ec) {
        if (m_errorHandler)
          m_errorHandler({ ec.message(), ErrorType::Handshake });
      }
      else {
        m_handshakePerformed = true;
        m_readyState = ReadyState::Open;

        if (m_connectHandler)
          m_connectHandler();

        // Start read cycle.
        PerformRead();

        // Perform writes, if enqueued.
        if (!m_writeRequests.empty())
          PerformWrite();

        // Perform pings, if enqueued.
        if (m_pingRequests > 0)
          PerformPing();

        // Perform close, if requested.
        if (m_closeRequested && !m_closeInProgress)
          PerformClose();
      }
    }
  );
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::PerformRead() {
  if (ReadyState::Closing == m_readyState || ReadyState::Closed == m_readyState) {
    return;
  }

  // Check if there are more bytes available than a header length (2).
  m_stream->async_read(m_bufferIn, [this](boostecr ec, size_t size) {
    if (boost::asio::error::operation_aborted == ec) {
      // Nothing to do.
    } else if (ec) {
      if (m_errorHandler)
        m_errorHandler({ec.message(), ErrorType::Receive});
    } else {
      string message{buffers_to_string(m_bufferIn.data())};

      if (m_stream->got_binary()) {
        // ISS:2906983
        typedef base64_from_binary<transform_width<const char *, 6, 8>> encode_base64;

        // NOTE: Encoding the base64 string makes the message's length different
        // from the 'size' argument.
        std::ostringstream os;
        std::copy(encode_base64(message.c_str()), encode_base64(message.c_str() + size), ostream_iterator<char>(os));
        message = os.str();

        auto padSize = ((4 - message.length()) % 4) % 4;
        message.append(padSize, '=');
      }

      if (m_readHandler)
        m_readHandler(size, std::move(message));

      m_bufferIn.consume(size);
    } // if (ec)

    // Enqueue another read.
    PerformRead();
  }); // async_read
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::PerformWrite() {
  assert(!m_writeRequests.empty());
  assert(!m_writeInProgress);
  m_writeInProgress = true;

  std::pair<string, bool> request = m_writeRequests.front();
  m_writeRequests.pop();

  m_stream->binary(request.second);

  // Auto-fragment disabled. Adjust write buffer to the largest message length
  // processed.
  if (request.first.length() > m_stream->write_buffer_bytes())
    m_stream->write_buffer_bytes(request.first.length());

  m_stream->async_write(buffer(request.first), [this](boostecr ec, size_t size) {
    if (ec) {
      if (m_errorHandler)
        m_errorHandler({ec.message(), ErrorType::Send});
    } else {
      if (m_writeHandler)
        m_writeHandler(size);
    }

    m_writeInProgress = false;

    if (!m_writeRequests.empty())
      PerformWrite();
  });
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::PerformPing() {
  assert(m_pingRequests > 0);
  assert(!m_pingInProgress);
  m_pingInProgress = true;

  --m_pingRequests;

  m_stream->async_ping(websocket::ping_data(), [this](boostecr ec) {
    if (ec) {
      if (m_errorHandler)
        m_errorHandler({ec.message(), ErrorType::Ping});
    } else if (m_pingHandler)
      m_pingHandler();

    m_pingInProgress = false;

    if (m_pingRequests > 0)
      PerformPing();
  });
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::PerformClose() {
  m_closeInProgress = true;
  m_readyState = ReadyState::Closing;

  m_stream->async_close(ToBeastCloseCode(m_closeCodeRequest), [this](boostecr ec) {
    if (ec) {
      if (m_errorHandler)
        m_errorHandler({ec.message(), ErrorType::Close});
    } else {
      m_readyState = ReadyState::Closed;

      if (m_closeHandler)
        m_closeHandler(m_closeCodeRequest, m_closeReasonRequest);
    }
  });

  // Synchronize context thread.
  Stop();
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Stop() {
  if (m_workGuard)
    m_workGuard->reset();

  if (m_contextThread.joinable() && std::this_thread::get_id() != m_contextThread.get_id())
    m_contextThread.join();
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::EnqueueWrite(const string &message, bool binary) {
  post(m_context, [this, message = std::move(message), binary]() {
    m_writeRequests.emplace(std::move(message), binary);

    if (!m_writeInProgress && ReadyState::Open == m_readyState)
      PerformWrite();
  });
}

template <typename SocketLayer, typename Stream, typename Resolver>
websocket::close_code BaseWebSocket<SocketLayer, Stream, Resolver>::ToBeastCloseCode(
    IWebSocket::CloseCode closeCode) {
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::Abnormal) == static_cast<uint16_t>(websocket::close_code::abnormal),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::BadPayload) ==
          static_cast<uint16_t>(websocket::close_code::bad_payload),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::GoingAway) ==
          static_cast<uint16_t>(websocket::close_code::going_away),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::InternalError) ==
          static_cast<uint16_t>(websocket::close_code::internal_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::NeedsExtension) ==
          static_cast<uint16_t>(websocket::close_code::needs_extension),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::None) == static_cast<uint16_t>(websocket::close_code::none),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::Normal) == static_cast<uint16_t>(websocket::close_code::normal),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::NoStatus) == static_cast<uint16_t>(websocket::close_code::no_status),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::PolicyError) ==
          static_cast<uint16_t>(websocket::close_code::policy_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::ProtocolError) ==
          static_cast<uint16_t>(websocket::close_code::protocol_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::Reserved1) ==
          static_cast<uint16_t>(websocket::close_code::reserved1),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::Reserved2) ==
          static_cast<uint16_t>(websocket::close_code::reserved2),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::Reserved3) ==
          static_cast<uint16_t>(websocket::close_code::reserved3),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::ServiceRestart) ==
          static_cast<uint16_t>(websocket::close_code::service_restart),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::TooBig) == static_cast<uint16_t>(websocket::close_code::too_big),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::TryAgainLater) ==
          static_cast<uint16_t>(websocket::close_code::try_again_later),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocket::CloseCode::UnknownData) ==
          static_cast<uint16_t>(websocket::close_code::unknown_data),
      "Exception type enums don't match");

  return static_cast<websocket::close_code>(closeCode);
}

#pragma region IWebSocket members

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Connect(
    const Protocols &protocols,
    const Options &options) {
  // "Cannot call Connect more than once");
  assert(ReadyState::Connecting == m_readyState);

  if (!protocols.empty()) {
    for (auto &protocol : protocols) {
      // ISS:2152951 - collect protocols
    }
  }

  Resolver resolver(m_context);
  resolver.async_resolve(
      m_url.host,
      m_url.port,
      [this, options = std::move(options)](boostecr ec, typename Resolver::results_type results) {
        if (ec) {
          if (m_errorHandler)
            m_errorHandler({ec.message(), ErrorType::Resolution});

          return;
        }

        // Connect
        get_lowest_layer(*m_stream).async_connect(
          results,
          [this, options = std::move(options)](boostecr ec, tcp::resolver::results_type::endpoint_type)
          {
            if (ec) {
              if (m_errorHandler)
                m_errorHandler({ ec.message(), ErrorType::Connection });
            }
            else {
              Handshake(std::move(options));
            }
          }
        );
      }); // async_resolve

  m_contextThread = std::thread([this]() {
    m_workGuard = make_unique<executor_work_guard<io_context::executor_type>>(make_work_guard(m_context));
    m_context.run();
  });
} // void Connect

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Close(CloseCode code, const string &reason) {
  if (m_closeRequested)
    return;

  m_closeRequested = true;
  m_closeCodeRequest = code;
  m_closeReasonRequest = std::move(reason);

  assert(!m_closeInProgress);
  // Give priority to Connect().
  if (m_handshakePerformed)
    PerformClose();
  else
    Stop(); // Synchronize the context thread.
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Send(const string &message) {
  EnqueueWrite(std::move(message), false);
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SendBinary(const string &base64String) {
  m_stream->binary(true);

  string message;
  try {
    typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> decode_base64;

    // A correctly formed base64 string should have from 0 to three '=' trailing
    // characters. Skip those.
    size_t padSize = std::count(base64String.begin(), base64String.end(), '=');
    message = string(decode_base64(base64String.begin()), decode_base64(base64String.end() - padSize));
  } catch (const std::exception &) {
    if (m_errorHandler)
      m_errorHandler({"", ErrorType::Send});

    return;
  }

  EnqueueWrite(std::move(message), true);
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::Ping() {
  if (ReadyState::Closed == m_readyState)
    return;

  ++m_pingRequests;
  if (!m_pingInProgress && ReadyState::Open == m_readyState)
    PerformPing();
}

#pragma endregion IWebSocket members

#pragma region Handler setters

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnConnect(function<void()> &&handler) {
  m_connectHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnPing(function<void()> &&handler) {
  m_pingHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnSend(function<void(size_t)> &&handler) {
  m_writeHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnMessage(
    function<void(size_t, const string &)> &&handler) {
  m_readHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnClose(
    function<void(CloseCode, const string &)> &&handler) {
  m_closeHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<SocketLayer, Stream, Resolver>::SetOnError(function<void(Error &&)> &&handler) {
  m_errorHandler = handler;
}

template <typename SocketLayer, typename Stream, typename Resolver>
IWebSocket::ReadyState BaseWebSocket<SocketLayer, Stream, Resolver>::GetReadyState() const {
  return m_readyState;
}

#pragma endregion Handler setters

#pragma endregion BaseWebSocket members

#pragma region WebSocket members

WebSocket::WebSocket(Url &&url) : BaseWebSocket(std::move(url)) {
  this->m_stream = make_unique<websocket::stream<boost::beast::tcp_stream>>(this->m_context);
  this->m_stream->auto_fragment(false); // ISS:2906963 Re-enable message fragmenting.
}

#pragma endregion

#pragma region SecureWebSocket members

SecureWebSocket::SecureWebSocket(Url &&url) : BaseWebSocket(std::move(url)) {
  auto ssl = ssl::context(ssl::context::sslv23_client);
  this->m_stream = make_unique<websocket::stream<ssl_stream<tcp_stream>>>(this->m_context, ssl);
  this->m_stream->auto_fragment(false); // ISS:2906963 Re-enable message fragmenting.
}

void SecureWebSocket::Handshake(const IWebSocket::Options &options) {
  this->m_stream->next_layer().async_handshake(
      ssl::stream_base::client, [this, options = std::move(options)](boostecr ec) {
        if (ec && this->m_errorHandler) {
          this->m_errorHandler({ec.message(), IWebSocket::ErrorType::Connection});
        } else {
          BaseWebSocket::Handshake(std::move(options));
        }
      });
}

#pragma endregion SecureWebSocket members

#pragma region IWebSocket static members

/*static*/ unique_ptr<IWebSocket> IWebSocket::Make(const string &urlString) {
  Url url(urlString);

  if (url.scheme == "ws") {
    if (url.port.empty())
      url.port = "80";

    return unique_ptr<IWebSocket>(new WebSocket(std::move(url)));
  } else if (url.scheme == "wss") {
    if (url.port.empty())
      url.port = "443";

    return unique_ptr<IWebSocket>(new SecureWebSocket(std::move(url)));
  } else
    throw std::exception((string("Incorrect url protocol: ") + url.scheme).c_str());
}

#pragma endregion IWebSocket static members

namespace Test {

#pragma region MockStream

MockStream::MockStream(io_context &context)
    : m_context{context},
      ConnectResult{[]() { return error_code{}; }},
      HandshakeResult{[](string, string) { return error_code{}; }},
      ReadResult{[]() { return std::make_pair<error_code, size_t>({}, 0); }},
      CloseResult{[]() { return error_code{}; }} {}

io_context::executor_type MockStream::get_executor() noexcept {
  return m_context.get_executor();
}

MockStream::lowest_layer_type &MockStream::lowest_layer() {
  return *this;
}

MockStream::lowest_layer_type const &MockStream::lowest_layer() const {
  return *this;
}

void MockStream::binary(bool value) {}

bool MockStream::got_binary() const {
  return false;
}

bool MockStream::got_text() const {
  return !got_binary();
}

void MockStream::write_buffer_bytes(size_t amount) {}

size_t MockStream::write_buffer_bytes() const {
  return 8;
}

void MockStream::set_option(websocket::stream_base::decorator opt) {}

void MockStream::get_option(websocket::stream_base::timeout& opt) {}

void MockStream::set_option(websocket::stream_base::timeout const& opt) {}

void MockStream::set_option(websocket::permessage_deflate const& o) {}

void MockStream::get_option(websocket::permessage_deflate& o) {}

template<class RangeConnectHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(RangeConnectHandler, void(error_code, tcp::resolver::results_type::endpoint_type))
MockStream::async_connect(
  tcp::resolver::results_type const& endpoints,
  RangeConnectHandler&& handler)
{
   //async_initiate<RangeConnectHandler,
   // void(error_code, typename tcp::resolver::results_type::endpoint_type)>(
   //   [](auto&&) {},
   //   handler
   // );

  //return async_initiate<RangeConnectHandler, void(error_code, tcp::resolver::results_type::endpoint_type)>(
  //  [](RangeConnectHandler&& handler, MockStream* ms, tcp::resolver::results_type::endpoint_type eps)
  //  {
  //    //post(ms->get_executor(), bind_handler(std::move(handler), ms->ConnectResult(), {} /*endpoint_type*/ ));
  //    post(ms->get_executor(), bind_handler(std::move(handler), ms->ConnectResult(), {}));
  //  },
  //  handler,
  //  this,
  //  {}
  //  );
}

template <class HandshakeHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(HandshakeHandler, void(error_code))
MockStream::async_handshake(
    boost::string_view host,
    boost::string_view target,
    HandshakeHandler &&handler) {
  //BOOST_BEAST_HANDLER_INIT(HandshakeHandler, void(error_code));
  //post(
  //    get_executor(),
  //    bind_handler(std::move(init.completion_handler), HandshakeResult(host.to_string(), target.to_string())));

  //return init.result.get();

  return async_initiate<HandshakeHandler, void(error_code)>(
    [](HandshakeHandler&& handler, MockStream* ms, string h, string t)
    {
      post(ms->get_executor(), bind_handler(std::move(handler), ms->HandshakeResult(h, t)));
    },
    handler,
    this,
    host.to_string(),
    target.to_string()
    );
}

template <class DynamicBuffer, class ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(error_code, size_t))
MockStream::async_read(DynamicBuffer &buffer, ReadHandler &&handler) {
  error_code ec;
  size_t size;
  std::tie(ec, size) = ReadResult();

  //BOOST_BEAST_HANDLER_INIT(ReadHandler, void(error_code, size_t));
  //post(get_executor(), bind_handler(std::move(init.completion_handler), ec, size));

  //return init.result.get();

  return async_initiate<ReadHandler, void(error_code, size_t)>(
      [ec, size](ReadHandler&& handler, MockStream* ms)
      {
        post(ms->get_executor(), bind_handler(std::move(handler), ec, size));
      },
      handler,
      this
    );
}

template <class ConstBufferSequence, class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code, size_t))
MockStream::async_write(ConstBufferSequence const &buffers, WriteHandler &&handler) {
  error_code ec;
  size_t size;
  std::tie(ec, size) = ReadResult();//TODO: Why in async_write?

  //BOOST_BEAST_HANDLER_INIT(WriteHandler, void(error_code, size_t));
  //post(get_executor(), bind_handler(std::move(init.completion_handler), ec, size));

  //return init.result.get();

  return async_initiate<WriteHandler, void(error_code, size_t)>(
      [ec, size](WriteHandler&& handler, MockStream* ms)
      {
        post(ms->get_executor(), bind_handler(std::move(handler), ec, size));
      },
      handler,
      this
    );
}

template <class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code))
MockStream::async_ping(websocket::ping_data const &payload, WriteHandler &&handler) {
  //BOOST_BEAST_HANDLER_INIT(WriteHandler, void(error_code));
  //post(get_executor(), bind_handler(std::move(init.completion_handler), PingResult()));

  //return init.result.get();

  return async_initiate<WriteHandler, void(error_code)>(
    [](WriteHandler&& handler, MockStream* ms)
    {
      post(ms->get_executor(), bind_handler(std::move(handler), ms->PingResult()));
    },
    handler,
    this
    );
}

template <class CloseHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(CloseHandler, void(error_code))
MockStream::async_close(websocket::close_reason const &cr, CloseHandler &&handler) {
  //BOOST_BEAST_HANDLER_INIT(CloseHandler, void(error_code));
  //post(get_executor(), bind_handler(std::move(init.completion_handler), CloseResult()));

  //return init.result.get();

  return async_initiate<CloseHandler, void(error_code)>(
    [](CloseHandler&& handler, MockStream* ms)
    {
      post(ms->get_executor(), bind_handler(std::move(handler), ms->CloseResult()));
    },
    handler,
    this
    );
}

#pragma endregion MockStream

#pragma region TestWebSocket

TestWebSocket::TestWebSocket(Url &&url) : BaseWebSocket(std::move(url)) {
  m_stream = make_unique<MockStream>(m_context);
}

void TestWebSocket::SetConnectResult(function<error_code()> &&resultFunc) {
  m_stream->ConnectResult = std::move(resultFunc);
}

void TestWebSocket::SetHandshakeResult(function<error_code(string, string)> &&resultFunc) {
  m_stream->HandshakeResult = std::move(resultFunc);
}

void TestWebSocket::SetCloseResult(function<error_code()> &&resultFunc) {
  m_stream->CloseResult = std::move(resultFunc);
}

#pragma endregion TestWebSocket

} // namespace Test

} // namespace Microsoft::React

namespace boost {

namespace asio {

//TODO: Remove
// See <boost/asio/connect.hpp>(776)
template <typename Iterator, typename IteratorConnectHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(IteratorConnectHandler, void(error_code, Iterator))
async_connect(
    Microsoft::React::Test::MockStream &s,
    Iterator begin,
    Iterator end,
    BOOST_ASIO_MOVE_ARG(IteratorConnectHandler) handler) {
  handler(s.ConnectResult(), {});
}

} // namespace boost::asio

namespace beast {

Microsoft::React::Test::MockStream::lowest_layer_type&
get_lowest_layer(Microsoft::React::Test::MockStream& s) noexcept
{
  return s.lowest_layer();
}

} // namespace boost::beast

} // namespace boost

#pragma warning(pop)
