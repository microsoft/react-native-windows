// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <queue>
#include <thread>
#include "IWebSocketResource.h"
#include "Utils.h"

namespace Microsoft::React {

namespace Beast {

template <
    typename SocketLayer = boost::beast::tcp_stream,
    typename Stream = boost::beast::websocket::stream<SocketLayer>>
class BaseWebSocketResource : public IWebSocketResource {
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string &, bool)> m_readHandler;
  std::function<void(CloseCode, const std::string &)> m_closeHandler;

  Url m_url;
  ReadyState m_readyState{ReadyState::Connecting};
  boost::beast::multi_buffer m_bufferIn;
  std::thread m_contextThread;

  /// <remarks>
  /// Must be modified exclusively from the context thread.
  /// </remarks>
  std::queue<std::pair<std::string, bool>> m_writeRequests;

  std::atomic_size_t m_pingRequests{0};
  CloseCode m_closeCodeRequest{CloseCode::Normal};
  std::string m_closeReasonRequest;

  // Internal status flags.
  std::atomic_bool m_handshakePerformed{false};
  std::atomic_bool m_closeRequested{false};
  std::atomic_bool m_closeInProgress{false};
  std::atomic_bool m_pingInProgress{false};
  std::atomic_bool m_writeInProgress{false};

  /// <summary>
  /// Add the message to a write queue for eventual sending.
  /// </summary>
  /// <param name="message">
  /// Payload to send to the remote endpoint.
  /// </param>
  /// <param name="binary">
  /// Indicates whether the payload should be treated as binary data, or text.
  /// </param>
  void EnqueueWrite(const std::string &message, bool binary);

  /// <summary>
  /// Dequeues a message from <c>m_writeRequests</c> and sends it
  /// asynchronously.
  /// </summary>
  void PerformWrite();

  /// <summary>
  /// If this instance is considered open, post a read request into
  /// <c>m_bufferIn</c>. If there is an incoming message and
  /// <c>m_readHandler</c> is set, call the handler. Then, post new call to this
  /// method to read further incoming data.
  /// </summary>
  void PerformRead();

  /// <summary>
  /// If there are pending ping requests, post an asynchronous ping.
  /// Invoke <c>m_pingHandler</c> if set.
  /// Call this method again if there are still pending requests.
  /// </summary>
  void PerformPing();

  /// <summary>
  /// Set the ready state to <c>Closing</c>.
  /// Post a close request for this stream.
  /// Stop this instance to drop any future read or write requests.
  /// </summary>
  void PerformClose();

  /// <summary>
  /// Synchronizes the context thread and allows the io_context to stop
  /// dispatching tasks.
  /// </summary>
  void Stop();

  boost::beast::websocket::close_code ToBeastCloseCode(IWebSocketResource::CloseCode closeCode);

#pragma region Async handlers

  void OnResolve(boost::beast::error_code ec, typename boost::asio::ip::tcp::resolver::results_type results);

  void OnConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoints);

  void OnHandshake(boost::beast::error_code ec);

  void OnClose(boost::beast::error_code ec);

  void OnRead(boost::beast::error_code ec, std::size_t size);

  void OnWrite(boost::beast::error_code ec, std::size_t size);

  void OnPing(boost::beast::error_code ec);

#pragma endregion Async handlers

 protected:
  /// <summary>
  /// See
  /// https://www.boost.org/doc/libs/1_68_0/doc/html/boost_asio/reference/io_context.html.
  ///
  /// Dispatches tasks posted either by <see cref="m_stream" />
  /// or arbitrary lambdas using <c>boost::asio::post</c>.
  /// </summary>
  /// <remarks>
  /// Tasks will be run in the thread that calls this object's <c>run</c>
  /// method.
  /// </remarks>
  boost::asio::io_context m_context;

  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;
  std::unique_ptr<Stream> m_stream;
  std::function<void(Error &&)> m_errorHandler;

  BaseWebSocketResource(Url &&url);

  /// <summary>
  /// Finalizes the connection setup to the remote endpoint.
  /// Sets the ready state to <c>Open</c>.
  /// </summary>
  /// <remarks>
  /// On callback, invokes the connect handler, if set.
  /// Performs a pending write, if requested during the connection process.
  /// Performs a pending ping call, if requested during the connection process.
  /// Closes this instance, if requested during the connection process.
  /// </remarks>
  /// <param name="options">
  /// Map of HTTP header fields sent by the remote endpoint.
  /// </param>
  virtual void Handshake();

  virtual std::shared_ptr<BaseWebSocketResource<SocketLayer, Stream>> SharedFromThis() = 0;

 public:
  ~BaseWebSocketResource() noexcept override;

#pragma region IWebSocketResource

  /// <summary>
  /// <see cref="IWebSocketResource::Connect" />
  /// </summary>
  void Connect(std::string &&url, const Protocols &protocols, const Options &options) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Ping" />
  /// </summary>
  void Ping() noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Send" />
  /// </summary>
  void Send(std::string &&message) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SendBinary" />
  /// </summary>
  void SendBinary(std::string &&base64String) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Close" />
  /// </summary>
  void Close(CloseCode code, const std::string &reason) noexcept override;

  ReadyState GetReadyState() const noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnConnect" />
  /// </summary>
  void SetOnConnect(std::function<void()> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnPing" />
  /// </summary>
  void SetOnPing(std::function<void()> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnSend" />
  /// </summary>
  void SetOnSend(std::function<void(std::size_t)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnMessage" />
  /// </summary>
  void SetOnMessage(std::function<void(std::size_t, const std::string &, bool isBinary)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnClose" />
  /// </summary>
  void SetOnClose(std::function<void(CloseCode, const std::string &)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnError" />
  /// </summary>
  void SetOnError(std::function<void(Error &&)> &&handler) noexcept override;

#pragma endregion IWebSocketResource
};

class WebSocketResource : public BaseWebSocketResource<>, public std::enable_shared_from_this<WebSocketResource> {
#pragma region BaseWebSocketResource overrides

  std::shared_ptr<BaseWebSocketResource<>> SharedFromThis() override;

#pragma endregion BaseWebSocketResource overrides

 public:
  WebSocketResource(Url &&url);
};

class SecureWebSocketResource : public BaseWebSocketResource<boost::beast::ssl_stream<boost::beast::tcp_stream>>,
                                public std::enable_shared_from_this<SecureWebSocketResource> {
#pragma region BaseWebSocketResource overrides

  void Handshake() override;

  void OnSslHandshake(boost::beast::error_code ec);

  std::shared_ptr<BaseWebSocketResource<boost::beast::ssl_stream<boost::beast::tcp_stream>>> SharedFromThis() override;

#pragma endregion BaseWebSocketResource overrides

 public:
  SecureWebSocketResource(Url &&url);
};

namespace Test {

/// <summary>
/// See <boost/beast/experimental/test/stream.hpp>.
/// </summary>
class MockStream {
  boost::asio::io_context &m_context;

 public:
  using next_layer_type = MockStream;
  using lowest_layer_type = MockStream;
  using executor_type = boost::asio::io_context::executor_type;

  MockStream(boost::asio::io_context &context);

  boost::asio::io_context::executor_type get_executor() noexcept;

  //  lowest_layer_type &lowest_layer();

  //  lowest_layer_type const &lowest_layer() const;

  void binary(bool value);

  bool got_binary() const;

  bool got_text() const;

  void write_buffer_bytes(std::size_t amount);

  std::size_t write_buffer_bytes() const;

  void set_option(boost::beast::websocket::stream_base::decorator opt);

  void get_option(boost::beast::websocket::stream_base::timeout &opt);

  void set_option(boost::beast::websocket::stream_base::timeout const &opt);

  void set_option(boost::beast::websocket::permessage_deflate const &o);

  void get_option(boost::beast::websocket::permessage_deflate &o);

#pragma region boost::beast::basic_stream mocks

  template <class RangeConnectHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(RangeConnectHandler, void(boost::system::error_code, boost::asio::ip::tcp::endpoint))
  async_connect(boost::asio::ip::tcp::resolver::iterator const &endpoints, RangeConnectHandler &&handler);

#pragma endregion boost::beast::basic_stream mocks

#pragma region boost::beast::websocket::stream mocks

  template <class HandshakeHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(HandshakeHandler, void(boost::system::error_code))
  async_handshake(boost::beast::string_view host, boost::beast::string_view target, HandshakeHandler &&handler);

  template <class DynamicBuffer, class ReadHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(boost::system::error_code, std::size_t))
  async_read(DynamicBuffer &buffer, ReadHandler &&handler);

  template <class ConstBufferSequence, class WriteHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(boost::system::error_code, std::size_t))
  async_write(ConstBufferSequence const &buffers, WriteHandler &&handler);

  template <class WriteHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(boost::system::error_code))
  async_ping(boost::beast::websocket::ping_data const &payload, WriteHandler &&handler);

  template <class CloseHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(CloseHandler, void(boost::system::error_code))
  async_close(boost::beast::websocket::close_reason const &cr, CloseHandler &&handler);

#pragma endregion boost::beast::websocket::stream mocks

  std::function<boost::system::error_code()> ConnectResult;
  std::function<boost::system::error_code(std::string, std::string)> HandshakeResult;
  std::function<std::pair<boost::system::error_code, std::size_t>()> ReadResult;
  std::function<std::pair<boost::system::error_code, std::size_t>()> WriteResult;
  std::function<boost::system::error_code()> PingResult;
  std::function<boost::system::error_code()> CloseResult;
};

class TestWebSocketResource : public BaseWebSocketResource<
                                  std::nullptr_t, // Unused. MockStream works as its own next/lowest layer.
                                  MockStream>,
                              public std::enable_shared_from_this<TestWebSocketResource> {
#pragma region BaseWebSocketResource overrides

  std::shared_ptr<BaseWebSocketResource<std::nullptr_t, MockStream>> SharedFromThis() override;

#pragma endregion BaseWebSocketResource overrides

 public:
  TestWebSocketResource(Url &&url);

  void SetConnectResult(std::function<boost::system::error_code()> &&resultFunc);
  void SetHandshakeResult(std::function<boost::system::error_code(std::string, std::string)> &&resultFunc);
  void SetCloseResult(std::function<boost::system::error_code()> &&resultFunc);
};

} // namespace Test

} // namespace Beast

class BeastWebSocketResource : public IWebSocketResource, public std::enable_shared_from_this<BeastWebSocketResource> {
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string &, bool)> m_readHandler;
  std::function<void(CloseCode, const std::string &)> m_closeHandler;
  std::function<void(Error &&)> m_errorHandler;

  std::shared_ptr<IWebSocketResource> m_concreteResource;

 public:
  BeastWebSocketResource() noexcept;

#pragma region IWebSocketResource

  /// <summary>
  /// <see cref="IWebSocketResource::Connect" />
  /// </summary>
  void Connect(std::string &&url, const Protocols &protocols, const Options &options) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Ping" />
  /// </summary>
  void Ping() noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Send" />
  /// </summary>
  void Send(std::string &&message) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SendBinary" />
  /// </summary>
  void SendBinary(std::string &&base64String) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::Close" />
  /// </summary>
  void Close(CloseCode code, const std::string &reason) noexcept override;

  ReadyState GetReadyState() const noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnConnect" />
  /// </summary>
  void SetOnConnect(std::function<void()> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnPing" />
  /// </summary>
  void SetOnPing(std::function<void()> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnSend" />
  /// </summary>
  void SetOnSend(std::function<void(std::size_t)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnMessage" />
  /// </summary>
  void SetOnMessage(std::function<void(std::size_t, const std::string &, bool isBinary)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnClose" />
  /// </summary>
  void SetOnClose(std::function<void(CloseCode, const std::string &)> &&handler) noexcept override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnError" />
  /// </summary>
  void SetOnError(std::function<void(Error &&)> &&handler) noexcept override;

#pragma endregion IWebSocketResource
};

} // namespace Microsoft::React
