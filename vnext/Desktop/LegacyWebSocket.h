// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <boost/beast/websocket.hpp>
#include <thread>
#include <queue>
#include "IWebSocket.h"
#include "Utils.h"

namespace facebook {
namespace react {

template<typename Protocol, typename Socket, typename Resolver>
class LegacyBaseWebSocket : public IWebSocket
{
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string&)> m_readHandler;
  std::function<void(CloseCode, const std::string&)> m_closeHandler;

  Url m_url;
  ReadyState m_readyState;
  boost::beast::multi_buffer m_bufferIn;
  std::atomic_bool m_closing;
  std::thread m_contextThread;
  std::queue<std::pair<std::string, bool>> m_requests;
  std::atomic_bool m_writeInProgress { false };

  void Read();
  void PerformWrite();
  void EnqueueWrite(const std::string& message, bool binary);
  boost::beast::websocket::close_code ToBeastCloseCode(IWebSocket::CloseCode closeCode);

protected:
  std::function<void(Error&&)> m_errorHandler;

  boost::asio::io_context m_context;
  Resolver m_resolver;
  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;
  std::unique_ptr<boost::beast::websocket::stream<Socket>> m_stream;

  LegacyBaseWebSocket(Url&& url);

  ~LegacyBaseWebSocket() override;

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
class LegacyWebSocket : public LegacyBaseWebSocket<Protocol, Socket, Resolver>
{
public:
  LegacyWebSocket(Url&& url);
};

template<typename Protocol, typename Socket, typename Resolver = boost::asio::ip::basic_resolver<Protocol>>
class LegacySecureWebSocket : public LegacyBaseWebSocket<Protocol, Socket, Resolver>
{
  #pragma region LegacyBaseWebSocket overrides

  void Handshake(const IWebSocket::Options& options) override;

  #pragma endregion

public:
  LegacySecureWebSocket(Url&& url);
};

} } // namespace facebook::react
