// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off

#pragma once

#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>
#include <IWebSocketResource.h>

#include <future>
#include <queue>

namespace Microsoft::React
{

class WinRTWebSocketResource : public IWebSocketResource
{
  winrt::Windows::Foundation::Uri m_uri;
  winrt::Windows::Networking::Sockets::MessageWebSocket m_socket;
  winrt::Windows::Networking::Sockets::MessageWebSocket::MessageReceived_revoker m_revoker;
  winrt::Windows::Storage::Streams::DataWriter m_writer{ m_socket.OutputStream() };

  std::atomic_bool m_connectRequested;
  std::atomic_bool m_sendRequested;
  std::promise<void> m_connectPerformed;
  std::promise<void> m_closePerformed;
  winrt::Windows::Foundation::IAsyncAction m_connectAction;

  CloseCode m_closeCode{ CloseCode::None };
  std::string m_closeReason;
  std::queue<std::pair<std::string, bool>> m_writeQueue;

  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string&)> m_readHandler;
  std::function<void(CloseCode, const std::string&)> m_closeHandler;
  std::function<void(Error&&)> m_errorHandler;

  WinRTWebSocketResource(winrt::Windows::Foundation::Uri&& uri);
  ~WinRTWebSocketResource() override;

  winrt::Windows::Foundation::IAsyncAction PerformConnect();
  winrt::fire_and_forget PerformPing();
  winrt::fire_and_forget PerformWrite();
  winrt::fire_and_forget PerformClose();

  void OnMessageReceived(
    winrt::Windows::Networking::Sockets::IWebSocket const& sender,
    winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs const& args);
  void OnClosed(
    winrt::Windows::Networking::Sockets::IWebSocket const& sender,
    winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs const& args);

  void Stop();

public:
  WinRTWebSocketResource(const std::string& urlString);

#pragma region IWebSocketResource

  /// <summary>
  /// <see cref="IWebSocketResource::Connect" />
  /// </summary>
  void Connect(const Protocols& protocols, const Options& options) override;

  /// <summary>
  /// <see cref="IWebSocketResource::Ping" />
  /// </summary>
  void Ping() override;

  /// <summary>
  /// <see cref="IWebSocketResource::Send" />
  /// </summary>
  void Send(const std::string& message) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SendBinary" />
  /// </summary>
  void SendBinary(const std::string& base64String) override;

  /// <summary>
  /// <see cref="IWebSocketResource::Close" />
  /// </summary>
  void Close(CloseCode code, const std::string& reason) override;

  ReadyState GetReadyState() const override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnConnect" />
  /// </summary>
  void SetOnConnect(std::function<void()>&& handler) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnPing" />
  /// </summary>
  void SetOnPing(std::function<void()>&& handler) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnSend" />
  /// </summary>
  void SetOnSend(std::function<void(std::size_t)>&& handler) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnMessage" />
  /// </summary>
  void SetOnMessage(std::function<void(std::size_t, const std::string&)>&& handler) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnClose" />
  /// </summary>
  void SetOnClose(std::function<void(CloseCode, const std::string&)>&& handler) override;

  /// <summary>
  /// <see cref="IWebSocketResource::SetOnError" />
  /// </summary>
  void SetOnError(std::function<void(Error&&)>&& handler) override;

#pragma endregion IWebSocketResource
};

} // namespace Microsoft::React
