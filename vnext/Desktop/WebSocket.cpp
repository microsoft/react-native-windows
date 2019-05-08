// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#pragma warning( push )
#pragma warning( disable : 4996 )  // std::copy::_Unchecked_iterators::_Deprecate

#include "WebSocket.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include "UnicodeConversion.h"

using namespace boost::archive::iterators;
using namespace boost::asio;
using namespace boost::beast;

using boost::asio::ip::basic_resolver_iterator;
using boost::asio::ip::tcp;

using std::make_unique;
using std::size_t;
using std::string;
using std::unique_ptr;

using boostecr = boost::system::error_code const&;

namespace Microsoft {
namespace React {

#pragma region BaseWebSocket members

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::BaseWebSocket(Url&& url)
  : m_url { std::move(url) }
{
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::~BaseWebSocket()
{
  if (!m_context.stopped())
  {
    if (!m_closeRequested)
      Close(CloseCode::GoingAway, "Terminating instance");
    else if (!m_closeInProgress)
      PerformClose();
  }
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Handshake(const IWebSocket::Options& options)
{
  m_stream->async_handshake_ex(m_url.host, m_url.Target(),
    // Header handler
    [options = std::move(options)](websocket::request_type& req)
  {
    // Collect headers
    for (const auto& header : options)
    {
      req.insert(facebook::react::UnicodeConversion::Utf16ToUtf8(header.first), header.second);
    }
  },
    // Handshake handler
    [this](boostecr ec)
  {
    if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Handshake });
    }
    else
    {
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
  }); // async_handshake_ex
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::PerformRead()
{
  if (ReadyState::Closing == m_readyState || ReadyState::Closed == m_readyState)
  {
    return;
  }

  // Check if there are more bytes available than a header length (2).
  m_stream->async_read(m_bufferIn, [this](boostecr ec, size_t size)
  {
    if (error::operation_aborted == ec)
    {
      // Nothing to do.
    }
    else if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Receive });
    }
    else
    {
      string message { buffers_to_string(m_bufferIn.data()) };

      if (m_stream->got_binary())
      {
        //ISS:2906983
        typedef base64_from_binary<transform_width<const char*, 6, 8>> encode_base64;

        //NOTE: Encoding the base64 string makes the message's length different from the 'size' argument.
        std::ostringstream os;
        std::copy(encode_base64(message.c_str()), encode_base64(message.c_str() + size), ostream_iterator<char>(os));
        message = os.str();

        auto padSize = ((4 - message.length()) % 4) % 4;
        message.append(padSize, '=');
      }

      if (m_readHandler)
        m_readHandler(size, std::move(message));

      m_bufferIn.consume(size);
    }// if (ec)

    // Enqueue another read.
    PerformRead();
  });// async_read
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::PerformWrite()
{
  assert(!m_writeRequests.empty());
  assert(!m_writeInProgress);
  m_writeInProgress = true;

  std::pair<string, bool> request = m_writeRequests.front();
  m_writeRequests.pop();

  m_stream->binary(request.second);

  // Auto-fragment disabled. Adjust write buffer to the largest message length processed.
  if (request.first.length() > m_stream->write_buffer_size())
    m_stream->write_buffer_size(request.first.length());

  m_stream->async_write(buffer(request.first), [this](boostecr ec, size_t size)
  {
    if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Send });
    }
    else
    {
      if (m_writeHandler)
        m_writeHandler(size);
    }

    m_writeInProgress = false;

    if (!m_writeRequests.empty())
      PerformWrite();
  });
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::PerformPing()
{
  assert(m_pingRequests > 0);
  assert(!m_pingInProgress);
  m_pingInProgress = true;

  --m_pingRequests;

  m_stream->async_ping(websocket::ping_data(), [this](boostecr ec)
  {
    if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Ping });
    }
    else if (m_pingHandler)
      m_pingHandler();

    m_pingInProgress = false;

    if (m_pingRequests > 0)
      PerformPing();
  });
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::PerformClose()
{
  m_closeInProgress = true;
  m_readyState = ReadyState::Closing;

  m_stream->async_close(ToBeastCloseCode(m_closeCodeRequest), [this](boostecr ec)
  {
    if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Close });
    }
    else
    {
      m_readyState = ReadyState::Closed;

      if (m_closeHandler)
        m_closeHandler(m_closeCodeRequest, m_closeReasonRequest);
    }
  });

  // Synchronize context thread.
  Stop();
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Stop()
{
  if (m_workGuard)
    m_workGuard->reset();

  if (m_contextThread.joinable() && std::this_thread::get_id() != m_contextThread.get_id())
    m_contextThread.join();
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::EnqueueWrite(const string& message, bool binary)
{
  post(m_context, [this, message = std::move(message), binary]()
  {
    m_writeRequests.emplace(std::move(message), binary);

    if (!m_writeInProgress && ReadyState::Open == m_readyState)
      PerformWrite();
  });
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
websocket::close_code BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::ToBeastCloseCode(IWebSocket::CloseCode closeCode)
{
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::Abnormal) == static_cast<uint16_t>(websocket::close_code::abnormal), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::BadPayload) == static_cast<uint16_t>(websocket::close_code::bad_payload), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::GoingAway) == static_cast<uint16_t>(websocket::close_code::going_away), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::InternalError) == static_cast<uint16_t>(websocket::close_code::internal_error), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::NeedsExtension) == static_cast<uint16_t>(websocket::close_code::needs_extension), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::None) == static_cast<uint16_t>(websocket::close_code::none), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::Normal) == static_cast<uint16_t>(websocket::close_code::normal), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::NoStatus) == static_cast<uint16_t>(websocket::close_code::no_status), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::PolicyError) == static_cast<uint16_t>(websocket::close_code::policy_error), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::ProtocolError) == static_cast<uint16_t>(websocket::close_code::protocol_error), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::Reserved1) == static_cast<uint16_t>(websocket::close_code::reserved1), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::Reserved2) == static_cast<uint16_t>(websocket::close_code::reserved2), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::Reserved3) == static_cast<uint16_t>(websocket::close_code::reserved3), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::ServiceRestart) == static_cast<uint16_t>(websocket::close_code::service_restart), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::TooBig) == static_cast<uint16_t>(websocket::close_code::too_big), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::TryAgainLater) == static_cast<uint16_t>(websocket::close_code::try_again_later), "Exception type enums don't match");
  static_assert(static_cast<uint16_t>(IWebSocket::CloseCode::UnknownData) == static_cast<uint16_t>(websocket::close_code::unknown_data), "Exception type enums don't match");

  return static_cast<websocket::close_code>(closeCode);
}

#pragma region IWebSocket members

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Connect(const Protocols& protocols, const Options& options)
{
  // "Cannot call Connect more than once");
  assert(ReadyState::Connecting == m_readyState);

  if (!protocols.empty())
  {
    for (auto& protocol : protocols)
    {
      //ISS:2152951 - collect protocols
    }
  }

  Resolver resolver(m_context);
  resolver.async_resolve(m_url.host, m_url.port, [this, options = std::move(options)](boostecr ec, typename Resolver::results_type results)
  {
    if (ec)
    {
      if (m_errorHandler)
        m_errorHandler({ ec.message(), ErrorType::Resolution });

      return;
    }

    // Connect
    async_connect(m_stream->lowest_layer(), results.begin(), results.end(), [this, options = std::move(options)](boostecr ec, const basic_resolver_iterator<Protocol>&)
    {
      if (ec)
      {
        if (m_errorHandler)
          m_errorHandler({ ec.message(), ErrorType::Connection });
      }
      else
      {
        Handshake(std::move(options));
      }
    }); // async_connect
  }); // async_resolve

  m_contextThread = std::thread([this]()
  {
    m_workGuard = make_unique<executor_work_guard<io_context::executor_type>>(make_work_guard(m_context));
    m_context.run();
  });
}// void Connect

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Close(CloseCode code, const string& reason)
{
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

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Send(const string& message)
{
  EnqueueWrite(std::move(message), false);
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SendBinary(const string& base64String)
{
  m_stream->binary(true);

  string message;
  try
  {
    typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> decode_base64;

    // A correctly formed base64 string should have from 0 to three '=' trailing characters. Skip those.
    size_t padSize = std::count(base64String.begin(), base64String.end(), '=');
    message = string(decode_base64(base64String.begin()), decode_base64(base64String.end() - padSize));
  }
  catch (const std::exception&)
  {
    if (m_errorHandler)
      m_errorHandler({ "", ErrorType::Send });

    return;
  }

  EnqueueWrite(std::move(message), true);
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::Ping()
{
  if (ReadyState::Closed == m_readyState)
    return;

  ++m_pingRequests;
  if (!m_pingInProgress && ReadyState::Open == m_readyState)
    PerformPing();
}

#pragma endregion // IWebSocket members

#pragma region Handler setters

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnConnect(std::function<void()>&& handler)
{
  m_connectHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnPing(std::function<void()>&& handler)
{
  m_pingHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnSend(std::function<void(size_t)>&& handler)
{
  m_writeHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnMessage(std::function<void(size_t, const string&)>&& handler)
{
  m_readHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnClose(std::function<void(CloseCode, const string&)>&& handler)
{
  m_closeHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
void BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::SetOnError(std::function<void(Error&&)>&& handler)
{
  m_errorHandler = handler;
}

template<typename Protocol, typename SocketLayer, typename Stream, typename Resolver>
IWebSocket::ReadyState BaseWebSocket<Protocol, SocketLayer, Stream, Resolver>::GetReadyState() const
{
  return m_readyState;
}

#pragma endregion // Handler setters

#pragma endregion // BaseWebSocket members

#pragma region WebSocket members

WebSocket::WebSocket(Url&& url)
  : BaseWebSocket(std::move(url))
{
  this->m_stream = make_unique<websocket::stream<basic_stream_socket<tcp>>>(this->m_context);
  this->m_stream->auto_fragment(false);//ISS:2906963 Re-enable message fragmenting.
}

#pragma endregion

#pragma region SecureWebSocket members

SecureWebSocket::SecureWebSocket(Url&& url)
  : BaseWebSocket(std::move(url))
{
  auto ssl = ssl::context(ssl::context::sslv23_client);
  this->m_stream = make_unique<websocket::stream<ssl::stream<tcp::socket>>>(this->m_context, ssl);
  this->m_stream->auto_fragment(false);//ISS:2906963 Re-enable message fragmenting.
}

void SecureWebSocket::Handshake(const IWebSocket::Options& options)
{
  this->m_stream->next_layer().async_handshake(ssl::stream_base::client, [this, options = std::move(options)](boostecr ec)
  {
    if (ec && this->m_errorHandler)
    {
      this->m_errorHandler({ ec.message(), IWebSocket::ErrorType::Connection });
    }
    else
    {
      BaseWebSocket::Handshake(std::move(options));
    }
  });
}

#pragma endregion // SecureWebSocket members

#pragma region IWebSocket static members

/*static*/ unique_ptr<IWebSocket> IWebSocket::Make(const string& urlString)
{
  Url url(urlString);

  if (url.scheme == "ws")
  {
    if (url.port.empty())
      url.port = "80";

    return unique_ptr<IWebSocket>(new WebSocket(std::move(url)));
  }
  else if (url.scheme == "wss")
  {
    if (url.port.empty())
      url.port = "443";

    return unique_ptr<IWebSocket>(new SecureWebSocket(std::move(url)));
  }
  else
    throw std::exception((string("Incorrect url protocol: ") + url.scheme).c_str());
}

#pragma endregion // IWebSocket static members

} } // namespace facebook::react

namespace Microsoft {
namespace React {
namespace Test {

#pragma region MockStreamLayer

MockStreamLayer::MockStreamLayer(io_context& context)
  : m_context{ context }
  , ConnectResult{ []() -> error_code { return error_code{}; } }
  , CloseResult{ []() -> error_code {return error_code{}; } }
{
}

io_context::executor_type MockStreamLayer::get_executor() noexcept
{
  return m_context.get_executor();
}

MockStreamLayer::lowest_layer_type& MockStreamLayer::lowest_layer()
{
  return *this;
}

MockStreamLayer::lowest_layer_type const& MockStreamLayer::lowest_layer() const
{
  return *this;
}

void MockStreamLayer::binary(bool value) {}

bool MockStreamLayer::got_binary() const
{
  return false;
}

bool MockStreamLayer::got_text() const
{
  return !got_binary();
}

void MockStreamLayer::auto_fragment(bool value) {}

bool MockStreamLayer::auto_fragment() const
{
  return false;
}

void MockStreamLayer::write_buffer_size(size_t amount) {}

size_t MockStreamLayer::write_buffer_size() const
{
  return 8;
}

#pragma region boost::beast::websocket::stream NextLayer methods

template<class DynamicBuffer, class ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(error_code, size_t))
MockStreamLayer::async_read(DynamicBuffer& buffer, ReadHandler&& handler)
{
  //TODO: Mock
}

template<class ConstBufferSequence, class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code, size_t))
MockStreamLayer::async_write(ConstBufferSequence const& buffers, WriteHandler&& handler)
{
  //TODO: Mock
}

template<class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code))
MockStreamLayer::async_ping(websocket::ping_data const& payload, WriteHandler&& handler)
{
  //TODO: Mock
}

template<class CloseHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(CloseHandler, void(error_code))
MockStreamLayer::async_close(websocket::close_reason const& cr, CloseHandler&& handler)
{
  BOOST_BEAST_HANDLER_INIT(CloseHandler, void(error_code));
  boost::asio::post(stream.get_executor(), bind_handler(std::move(init.completion_handler), CloseResult()));

  return init.result.get();
}

// AsyncStream compliance
template<class MutableBufferSequence, class ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(error_code, size_t))
MockStreamLayer::async_read_some(MutableBufferSequence const& buffers, ReadHandler&& handler)
{
  //TODO: Mock
}

template<class ConstBufferSequence, class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code, size_t))
MockStreamLayer::async_write_some(ConstBufferSequence const& buffers, WriteHandler&& handler)
{
  //TODO: Mock
}

#pragma endregion // boost::beast::websocket::stream NextLayer methods

#pragma endregion // MockStreamLayer

#pragma region TestWebSocketOld

TestWebSocketOld::TestWebSocketOld(facebook::react::Url&& url)
  : BaseWebSocket(std::move(url))
{
  m_stream = make_unique<websocket::stream<MockStreamLayer>>(m_context);
  m_stream->auto_fragment(false);//ISS:2906963 Re-enable message fragmenting.
}

void TestWebSocketOld::SetConnectResult(std::function<error_code()>&& resultFunc)
{
  m_stream->next_layer().ConnectResult = std::move(resultFunc);
}

void TestWebSocketOld::SetCloseResult(std::function<error_code()>&& resultFunc)
{
  m_stream->next_layer().CloseResult = std::move(resultFunc);
}

#pragma endregion

} } } // namespace Microsoft::React::Test

namespace boost {
namespace asio {

// See <boost/asio/connect.hpp>(776)
template
<
  typename Iterator,
  typename IteratorConnectHandler
>
BOOST_ASIO_INITFN_RESULT_TYPE(IteratorConnectHandler, void(boost::system::error_code, Iterator))
async_connect
(
  Microsoft::React::Test::MockStreamLayer& s,
  Iterator begin,
  Iterator end,
  BOOST_ASIO_MOVE_ARG(IteratorConnectHandler) handler
)
{
  handler(s.ConnectResult(), {});
}

} // namespace boost::asio

// HTTP low-level mock overrides.
namespace beast {
namespace http {

///
// msg - type http::message<0, http::basic_string_body<char, std::char_traits<char>, std::allocator<char>>, http::basic_fields<std::allocator<char>>>
///
template<class DynamicBuffer, bool isRequest, class Body, class Allocator, class ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(error_code, std::size_t))
async_read(Microsoft::React::Test::MockStreamLayer& stream, DynamicBuffer& buffer, message<isRequest, Body, basic_fields<Allocator>>& msg, ReadHandler&& handler)
{
  // Build response.
  // TODO: Make mockable?
  msg.result(status::switching_protocols);
  msg.version(11);
  msg.set(field::upgrade, "websocket");
  msg.set(field::connection, "upgrade");
  msg.set(field::sec_websocket_key, stream.Key); //TODO: Keep this, or accepted key (acc)?
  msg.set(field::sec_websocket_version, "13");
  msg.set(field::user_agent, BOOST_BEAST_VERSION_STRING);

  websocket::detail::sec_ws_accept_type acc;
  websocket::detail::make_sec_ws_accept(acc, stream.Key);
  msg.set(field::sec_websocket_accept, acc);

  BOOST_BEAST_HANDLER_INIT(ReadHandler, void(error_code, std::size_t));
  boost::asio::post(stream.get_executor(), bind_handler(std::move(init.completion_handler), boost::system::error_code{}, 0));

  return init.result.get();
}

///
// sr - type http::serializer<1, http::empty_body, http::basic_fields<std::allocator<char>>>
///
template<bool isRequest, class Body, class Fields, class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code, std::size_t))
async_write(Microsoft::React::Test::MockStreamLayer& stream, serializer<isRequest, Body, Fields>& sr, WriteHandler&& handler)
{
  stream.Key = sr.get().at(field::sec_websocket_key);

  BOOST_BEAST_HANDLER_INIT(WriteHandler, void(error_code, std::size_t));
  boost::asio::post(stream.get_executor(), bind_handler(std::move(init.completion_handler), boost::system::error_code{}, 0));

  return init.result.get();
}

} } // namespace boost::beast::http

} // namespace boost

#pragma warning(pop)
