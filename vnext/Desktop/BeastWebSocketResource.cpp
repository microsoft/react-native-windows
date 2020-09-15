// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "BeastWebSocketResource.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include "Unicode.h"

using namespace boost::archive::iterators;
using namespace boost::asio;
using namespace boost::beast;

using boost::asio::async_initiate;
using boost::asio::ip::tcp;

using std::function;
using std::make_unique;
using std::shared_ptr;
using std::size_t;
using std::string;
using std::unique_ptr;

namespace Microsoft::React {

namespace Beast {

#pragma region BaseWebSocketResource members

template <typename SocketLayer, typename Stream>
BaseWebSocketResource<SocketLayer, Stream>::BaseWebSocketResource(Url &&url) : m_url{std::move(url)} {}

template <typename SocketLayer, typename Stream>
BaseWebSocketResource<SocketLayer, Stream>::~BaseWebSocketResource() {
  if (!m_context.stopped()) {
    if (!m_closeRequested)
      Close(CloseCode::GoingAway, "Terminating instance");
    else if (!m_closeInProgress)
      PerformClose();
  }
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Handshake() {
  // TODO: Enable if we want a configurable timeout.
  // m_stream->set_option(websocket::stream_base::timeout::suggested(role_type::client));
  m_stream->async_handshake(
      m_url.host,
      m_url.Target(),
      bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnHandshake, SharedFromThis()));
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnHandshake(error_code ec) {
  if (ec) {
    if (m_errorHandler)
      m_errorHandler({ec.message(), ErrorType::Handshake});
  } else {
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

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::PerformRead() {
  if (ReadyState::Closing == m_readyState || ReadyState::Closed == m_readyState) {
    return;
  }

  // Check if there are more bytes available than a header length (2).
  m_stream->async_read(
      m_bufferIn, bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnRead, SharedFromThis()));
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnRead(error_code ec, size_t size) {
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
      m_readHandler(size, std::move(message), m_stream->got_binary());

    m_bufferIn.consume(size);
  } // if (ec)

  // Enqueue another read.
  PerformRead();
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::PerformWrite() {
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

  m_stream->async_write(
      buffer(request.first),
      bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnWrite, SharedFromThis()));
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnWrite(error_code ec, size_t size) {
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
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::PerformPing() {
  assert(m_pingRequests > 0);
  assert(!m_pingInProgress);
  m_pingInProgress = true;

  --m_pingRequests;

  m_stream->async_ping(
      websocket::ping_data(),
      bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnPing, SharedFromThis()));
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnPing(error_code ec) {
  if (ec) {
    if (m_errorHandler)
      m_errorHandler({ec.message(), ErrorType::Ping});
  } else if (m_pingHandler)
    m_pingHandler();

  m_pingInProgress = false;

  if (m_pingRequests > 0)
    PerformPing();
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::PerformClose() {
  m_closeInProgress = true;
  m_readyState = ReadyState::Closing;

  m_stream->async_close(
      ToBeastCloseCode(m_closeCodeRequest),
      bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnClose, SharedFromThis()));

  // Synchronize context thread.
  Stop();
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnClose(error_code ec) {
  if (ec) {
    if (m_errorHandler)
      m_errorHandler({ec.message(), ErrorType::Close});
  } else {
    m_readyState = ReadyState::Closed;

    if (m_closeHandler)
      m_closeHandler(m_closeCodeRequest, m_closeReasonRequest);
  }
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Stop() {
  if (m_workGuard)
    m_workGuard->reset();

  if (m_contextThread.joinable() && std::this_thread::get_id() != m_contextThread.get_id())
    m_contextThread.join();
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::EnqueueWrite(const string &message, bool binary) {
  post(m_context, [self = SharedFromThis(), message = std::move(message), binary]() {
    self->m_writeRequests.emplace(std::move(message), binary);

    if (!self->m_writeInProgress && ReadyState::Open == self->m_readyState)
      self->PerformWrite();
  });
}

template <typename SocketLayer, typename Stream>
websocket::close_code BaseWebSocketResource<SocketLayer, Stream>::ToBeastCloseCode(
    IWebSocketResource::CloseCode closeCode) {
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::Abnormal) ==
          static_cast<uint16_t>(websocket::close_code::abnormal),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::BadPayload) ==
          static_cast<uint16_t>(websocket::close_code::bad_payload),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::GoingAway) ==
          static_cast<uint16_t>(websocket::close_code::going_away),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::InternalError) ==
          static_cast<uint16_t>(websocket::close_code::internal_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::NeedsExtension) ==
          static_cast<uint16_t>(websocket::close_code::needs_extension),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::Normal) ==
          static_cast<uint16_t>(websocket::close_code::normal),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::NoStatus) ==
          static_cast<uint16_t>(websocket::close_code::no_status),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::PolicyError) ==
          static_cast<uint16_t>(websocket::close_code::policy_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::ProtocolError) ==
          static_cast<uint16_t>(websocket::close_code::protocol_error),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::Reserved1) ==
          static_cast<uint16_t>(websocket::close_code::reserved1),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::Reserved2) ==
          static_cast<uint16_t>(websocket::close_code::reserved2),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::Reserved3) ==
          static_cast<uint16_t>(websocket::close_code::reserved3),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::ServiceRestart) ==
          static_cast<uint16_t>(websocket::close_code::service_restart),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::TooBig) ==
          static_cast<uint16_t>(websocket::close_code::too_big),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::TryAgainLater) ==
          static_cast<uint16_t>(websocket::close_code::try_again_later),
      "Exception type enums don't match");
  static_assert(
      static_cast<uint16_t>(IWebSocketResource::CloseCode::UnknownData) ==
          static_cast<uint16_t>(websocket::close_code::unknown_data),
      "Exception type enums don't match");

  return static_cast<websocket::close_code>(closeCode);
}

#pragma region IWebSocketResource members

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Connect(const Protocols &protocols, const Options &options) noexcept {
  // "Cannot call Connect more than once");
  assert(ReadyState::Connecting == m_readyState);

  // TODO: Enable?
  // m_stream->set_option(websocket::stream_base::timeout::suggested(role_type::client));
  m_stream->set_option(websocket::stream_base::decorator([options = std::move(options)](websocket::request_type &req) {
    // Collect headers
    for (const auto &header : options) {
      req.insert(Microsoft::Common::Unicode::Utf16ToUtf8(header.first), header.second);
    }
  }));

  if (!protocols.empty()) {
    for (auto &protocol : protocols) {
      // ISS:2152951 - collect protocols
    }
  }

  tcp::resolver resolver(m_context);
  resolver.async_resolve(
      m_url.host,
      m_url.port,
      bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnResolve, SharedFromThis()));

  m_contextThread = std::thread([self = SharedFromThis()]() {
    self->m_workGuard = make_unique<executor_work_guard<io_context::executor_type>>(make_work_guard(self->m_context));
    self->m_context.run();
  });
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnResolve(
    error_code ec,
    typename tcp::resolver::results_type results) {
  if (ec) {
    if (m_errorHandler)
      m_errorHandler({ec.message(), ErrorType::Resolution});

    return;
  }

  // Connect
  get_lowest_layer(*m_stream).async_connect(
      results, bind_front_handler(&BaseWebSocketResource<SocketLayer, Stream>::OnConnect, SharedFromThis()));
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::OnConnect(
    error_code ec,
    tcp::resolver::results_type::endpoint_type endpoints) {
  if (ec) {
    if (m_errorHandler)
      m_errorHandler({ec.message(), ErrorType::Connection});
  } else {
    Handshake();
  }
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Close(CloseCode code, const string &reason) noexcept {
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

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Send(string &&message) noexcept {
  EnqueueWrite(std::move(message), false);
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SendBinary(string &&base64String) noexcept {
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

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::Ping() noexcept {
  if (ReadyState::Closed == m_readyState)
    return;

  ++m_pingRequests;
  if (!m_pingInProgress && ReadyState::Open == m_readyState)
    PerformPing();
}

#pragma endregion IWebSocketResource members

#pragma region Handler setters

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnConnect(function<void()> &&handler) noexcept {
  m_connectHandler = handler;
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnPing(function<void()> &&handler) noexcept {
  m_pingHandler = handler;
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnSend(function<void(size_t)> &&handler) noexcept {
  m_writeHandler = handler;
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnMessage(
    function<void(size_t, const string &, bool isBinary)> &&handler) noexcept {
  m_readHandler = handler;
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnClose(
    function<void(CloseCode, const string &)> &&handler) noexcept {
  m_closeHandler = handler;
}

template <typename SocketLayer, typename Stream>
void BaseWebSocketResource<SocketLayer, Stream>::SetOnError(function<void(Error &&)> &&handler) noexcept {
  m_errorHandler = handler;
}

template <typename SocketLayer, typename Stream>
IWebSocketResource::ReadyState BaseWebSocketResource<SocketLayer, Stream>::GetReadyState() const noexcept {
  return m_readyState;
}

#pragma endregion Handler setters

#pragma endregion BaseWebSocketResource members

#pragma region WebSocketResource members

WebSocketResource::WebSocketResource(Url &&url) : BaseWebSocketResource(std::move(url)) {
  this->m_stream = make_unique<websocket::stream<boost::beast::tcp_stream>>(this->m_context);
  this->m_stream->auto_fragment(false); // ISS:2906963 Re-enable message fragmenting.
}

shared_ptr<BaseWebSocketResource<>> WebSocketResource::SharedFromThis() /*override*/
{
  return this->shared_from_this();
}

#pragma endregion WebSocketResource members

#pragma region SecureWebSocketResource members

SecureWebSocketResource::SecureWebSocketResource(Url &&url) : BaseWebSocketResource(std::move(url)) {
  auto ssl = ssl::context(ssl::context::sslv23_client);
  this->m_stream = make_unique<websocket::stream<ssl_stream<tcp_stream>>>(this->m_context, ssl);
  this->m_stream->auto_fragment(false); // ISS:2906963 Re-enable message fragmenting.
}

void SecureWebSocketResource::Handshake() {
  // Prefer shared_from_this() in concrete classes. SharedFromThis() falis to compile.
  this->m_stream->next_layer().async_handshake(
      ssl::stream_base::client, bind_front_handler(&SecureWebSocketResource::OnSslHandshake, shared_from_this()));
}

void SecureWebSocketResource::OnSslHandshake(error_code ec) {
  if (ec && m_errorHandler) {
    m_errorHandler({ec.message(), ErrorType::Connection});
  } else {
    BaseWebSocketResource::Handshake();
  }
}

shared_ptr<BaseWebSocketResource<ssl_stream<tcp_stream>>> SecureWebSocketResource::SharedFromThis()
/*override*/ {
  return this->shared_from_this();
}

#pragma endregion SecureWebSocketResource members

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

void MockStream::get_option(websocket::stream_base::timeout &opt) {}

void MockStream::set_option(websocket::stream_base::timeout const &opt) {}

void MockStream::set_option(websocket::permessage_deflate const &o) {}

void MockStream::get_option(websocket::permessage_deflate &o) {}

template <class RangeConnectHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(RangeConnectHandler, void(error_code, tcp::endpoint))
MockStream::async_connect(tcp::resolver::iterator const &endpoints, RangeConnectHandler &&handler) {
  return async_initiate<RangeConnectHandler, void(error_code, tcp::endpoint)>(
      [](RangeConnectHandler &&handler, MockStream *ms, tcp::resolver::iterator it) {
        tcp::endpoint ep; // TODO: make modifiable.
        post(ms->get_executor(), bind_handler(std::move(handler), ms->ConnectResult(), ep));
      },
      handler,
      this,
      endpoints);
}

template <class HandshakeHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(HandshakeHandler, void(error_code))
MockStream::async_handshake(boost::string_view host, boost::string_view target, HandshakeHandler &&handler) {
  return async_initiate<HandshakeHandler, void(error_code)>(
      [](HandshakeHandler &&handler, MockStream *ms, string h, string t) {
        post(ms->get_executor(), bind_handler(std::move(handler), ms->HandshakeResult(h, t)));
      },
      handler,
      this,
      host.to_string(),
      target.to_string());
}

template <class DynamicBuffer, class ReadHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(error_code, size_t))
MockStream::async_read(DynamicBuffer &buffer, ReadHandler &&handler) {
  error_code ec;
  size_t size;
  std::tie(ec, size) = ReadResult();

  return async_initiate<ReadHandler, void(error_code, size_t)>(
      [ec, size](ReadHandler &&handler, MockStream *ms) {
        post(ms->get_executor(), bind_handler(std::move(handler), ec, size));
      },
      handler,
      this);
}

template <class ConstBufferSequence, class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code, size_t))
MockStream::async_write(ConstBufferSequence const &buffers, WriteHandler &&handler) {
  error_code ec;
  size_t size;
  std::tie(ec, size) = ReadResult(); // TODO: Why in async_write?

  return async_initiate<WriteHandler, void(error_code, size_t)>(
      [ec, size](WriteHandler &&handler, MockStream *ms) {
        post(ms->get_executor(), bind_handler(std::move(handler), ec, size));
      },
      handler,
      this);
}

template <class WriteHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(error_code))
MockStream::async_ping(websocket::ping_data const &payload, WriteHandler &&handler) {
  return async_initiate<WriteHandler, void(error_code)>(
      [](WriteHandler &&handler, MockStream *ms) {
        post(ms->get_executor(), bind_handler(std::move(handler), ms->PingResult()));
      },
      handler,
      this);
}

template <class CloseHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(CloseHandler, void(error_code))
MockStream::async_close(websocket::close_reason const &cr, CloseHandler &&handler) {
  return async_initiate<CloseHandler, void(error_code)>(
      [](CloseHandler &&handler, MockStream *ms) {
        post(ms->get_executor(), bind_handler(std::move(handler), ms->CloseResult()));
      },
      handler,
      this);
}

#pragma endregion MockStream

#pragma region TestWebSocket

shared_ptr<Beast::BaseWebSocketResource<std::nullptr_t, MockStream>>
TestWebSocketResource::SharedFromThis() /*override*/
{
  return this->shared_from_this();
}

TestWebSocketResource::TestWebSocketResource(Url &&url) : BaseWebSocketResource(std::move(url)) {
  m_stream = make_unique<MockStream>(m_context);
}

void TestWebSocketResource::SetConnectResult(function<error_code()> &&resultFunc) {
  m_stream->ConnectResult = std::move(resultFunc);
}

void TestWebSocketResource::SetHandshakeResult(function<error_code(string, string)> &&resultFunc) {
  m_stream->HandshakeResult = std::move(resultFunc);
}

void TestWebSocketResource::SetCloseResult(function<error_code()> &&resultFunc) {
  m_stream->CloseResult = std::move(resultFunc);
}

#pragma endregion TestWebSocket
} // namespace Test

} // namespace Beast

} // namespace Microsoft::React

namespace boost::beast {

Microsoft::React::Beast::Test::MockStream::lowest_layer_type &get_lowest_layer(
    Microsoft::React::Beast::Test::MockStream &s) noexcept {
  return s;
}

} // namespace boost::beast
