// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <thread>
#include <queue>
#include "IWebSocket.h"
#include "Utils.h"

namespace Microsoft {
namespace React {

template
<
  typename Protocol     = boost::asio::ip::tcp,
  typename SocketLayer  = boost::asio::basic_stream_socket<Protocol>,
  typename Stream       = boost::beast::websocket::stream<SocketLayer>,
  typename Resolver     = boost::asio::ip::basic_resolver<Protocol>
>
class BaseWebSocket : public IWebSocket
{
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string&)> m_readHandler;
  std::function<void(CloseCode, const std::string&)> m_closeHandler;

  Url m_url;
  ReadyState m_readyState { ReadyState::Connecting };
  boost::beast::multi_buffer m_bufferIn;
  std::thread m_contextThread;

  ///
  // Must be modified exclusively from the context thread.
  ///
  std::queue<std::pair<std::string, bool>> m_writeRequests;

  std::atomic_size_t m_pingRequests { 0 };
  CloseCode m_closeCodeRequest { CloseCode::None };
  std::string m_closeReasonRequest;

  // Internal status flags.
  std::atomic_bool m_handshakePerformed { false };
  std::atomic_bool m_closeRequested { false };
  std::atomic_bool m_closeInProgress { false };
  std::atomic_bool m_pingInProgress { false };
  std::atomic_bool m_writeInProgress { false };

  void EnqueueWrite(const std::string& message, bool binary);
  void PerformWrite();
  void PerformRead();
  void PerformPing();
  void PerformClose();

  ///
  // Synchronizes the context thread and allows the io_context to stop dispatching tasks.
  ///
  void Stop();

  boost::beast::websocket::close_code ToBeastCloseCode(IWebSocket::CloseCode closeCode);

protected:
  std::function<void(Error&&)> m_errorHandler;

  boost::asio::io_context m_context;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;
  std::unique_ptr<Stream> m_stream;

  BaseWebSocket(Url&& url);

  ~BaseWebSocket() override;

  virtual void Handshake(const IWebSocket::Options& options);

public:
  #pragma region IWebSocket members

  void Connect(const Protocols& protocols, const Options& options) override;
  void Ping() override;
  void Send(const std::string& message) override;
  void SendBinary(const std::string& base64String) override;
  void Close(CloseCode code, const std::string& reason) override;

  ReadyState GetReadyState() const override;

  void SetOnConnect(std::function<void()>&& handler) override;
  void SetOnPing(std::function<void()>&& handler) override;
  void SetOnSend(std::function<void(std::size_t)>&& handler) override;
  void SetOnMessage(std::function<void(std::size_t, const std::string&)>&& handler) override;
  void SetOnClose(std::function<void(CloseCode, const std::string&)>&& handler) override;
  void SetOnError(std::function<void(Error&&)>&& handler) override;

  #pragma endregion
};

class WebSocket
  : public  BaseWebSocket<>
{
public:
  WebSocket(Url&& url);
};

class SecureWebSocket : public BaseWebSocket<boost::asio::ip::tcp, boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
{
  #pragma region BaseWebSocket overrides

  void Handshake(const IWebSocket::Options& options) override;

  #pragma endregion

public:
  SecureWebSocket(Url&& url);
};

namespace Test {

// See <boost/beast/experimental/test/stream.hpp>
class MockStream
{
  boost::asio::io_context& m_context;

public:
  using next_layer_type = MockStream;
  using lowest_layer_type = MockStream;
  using executor_type = boost::asio::io_context::executor_type;

  MockStream(boost::asio::io_context& context);

  boost::asio::io_context::executor_type get_executor() noexcept;

  lowest_layer_type& lowest_layer();

  lowest_layer_type const& lowest_layer() const;

  void binary(bool value);

  bool got_binary() const;

  bool got_text() const;

  void write_buffer_size(std::size_t amount);

  std::size_t write_buffer_size() const;

  #pragma region boost::beast::websocket::stream mocks

  template<class RequestDecorator, class HandshakeHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(HandshakeHandler, void(boost::system::error_code))
  async_handshake_ex(
    boost::beast::string_view host,
    boost::beast::string_view target,
    RequestDecorator const& decorator,
    HandshakeHandler&& handler);

  template<class DynamicBuffer, class ReadHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(ReadHandler, void(boost::system::error_code, std::size_t))
  async_read(DynamicBuffer& buffer, ReadHandler&& handler);

  template<class ConstBufferSequence, class WriteHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(boost::system::error_code, std::size_t))
  async_write(ConstBufferSequence const& buffers, WriteHandler&& handler);

  template<class WriteHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WriteHandler, void(boost::system::error_code))
  async_ping(boost::beast::websocket::ping_data const& payload, WriteHandler&& handler);

  template<class CloseHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(CloseHandler, void(boost::system::error_code))
  async_close(boost::beast::websocket::close_reason const& cr, CloseHandler&& handler);

  #pragma endregion // boost::beast::websocket::stream mocks

  std::function<boost::system::error_code()> ConnectResult;
  std::function<boost::system::error_code()> HandshakeResult;
  std::function<std::pair<boost::system::error_code, std::size_t>()> ReadResult;
  std::function<std::pair<boost::system::error_code, std::size_t>()> WriteResult;
  std::function<boost::system::error_code()> PingResult;
  std::function<boost::system::error_code()> CloseResult;
};

class TestWebSocket
  : public  BaseWebSocket
            <
              boost::asio::ip::tcp, // TODO: Mock this and Resolver.
              std::nullptr_t,       // Unused. MockStream works as its own next/lowest layer.
              MockStream
            >
{
public:
  TestWebSocket(facebook::react::Url&& url);

  void SetConnectResult(std::function<boost::system::error_code()>&& resultFunc);
  void SetHandshakeResult(std::function<boost::system::error_code()>&& resultFunc);
  void SetCloseResult(std::function<boost::system::error_code()>&& resultFunc);
};

} // namespace Microsoft::React::Test

} } // namespace Microsoft::React
