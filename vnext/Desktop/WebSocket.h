// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <boost/beast/websocket.hpp>
#include <thread>
#include <queue>
#include "IWebSocket.h"
#include "Utils.h"

#include <boost/beast/experimental/test/stream.hpp>

namespace facebook {
namespace react {

template<typename Protocol, typename Socket, typename Resolver>
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
  std::unique_ptr<boost::beast::websocket::stream<Socket>> m_stream;

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

template<typename Protocol, typename Socket = boost::asio::basic_stream_socket<Protocol>, typename Resolver = boost::asio::ip::basic_resolver<Protocol>>
class WebSocket : public BaseWebSocket<Protocol, Socket, Resolver>
{
public:
  WebSocket(Url&& url);
};

template<typename Protocol, typename Socket, typename Resolver = boost::asio::ip::basic_resolver<Protocol>>
class SecureWebSocket : public BaseWebSocket<Protocol, Socket, Resolver>
{
  #pragma region BaseWebSocket overrides

  void Handshake(const IWebSocket::Options& options) override;

  #pragma endregion

public:
  SecureWebSocket(Url&& url);
};

} } // namespace facebook::react

namespace Microsoft {
namespace React {
namespace Test {

class MockStreamLayer : public boost::beast::test::stream
{
  friend
  void
    teardown(
      boost::beast::websocket::role_type,
      Microsoft::React::Test::MockStreamLayer& s,
      boost::system::error_code& ec) {}

  template<class TeardownHandler>
  friend
  void
    async_teardown(
      boost::beast::websocket::role_type role,
      Microsoft::React::Test::MockStreamLayer& s,
      TeardownHandler&& handler) {}

public:
  MockStreamLayer(boost::asio::io_context& context)
    : boost::beast::test::stream{ context }
  {
  }

  std::function<boost::system::error_code()> ConnectResult;

  //TODO: Verify it doesn't conflict with base class, then delete.
  //using lowest_layer_type = MockStreamLayer;

  //lowest_layer_type&
  //  lowest_layer()
  //{
  //  return *this;
  //}

  //lowest_layer_type const&
  //  lowest_layer() const
  //{
  //  return *this;
  //}
};

class TestWebSocketOld : public facebook::react::BaseWebSocket<boost::asio::ip::tcp, boost::beast::test::stream, boost::asio::ip::basic_resolver<boost::asio::ip::tcp>>
{
public:
  TestWebSocketOld(facebook::react::Url&& url);
};

class TestWebSocket : public facebook::react::BaseWebSocket<boost::asio::ip::tcp, MockStreamLayer, boost::asio::ip::basic_resolver<boost::asio::ip::tcp>>
{
public:
  TestWebSocket(facebook::react::Url&& url);

  void SetConnectResult(std::function<boost::system::error_code()>&& resultFunc);
};

} } } // namespace Microsoft::React::Test
