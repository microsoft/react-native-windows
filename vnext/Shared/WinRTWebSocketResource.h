// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IWebSocketResource.h>
#include <dispatchQueue/dispatchQueue.h>
#include <eventWaitHandle/eventWaitHandle.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

// Standard Library
#include <future>
#include <mutex>
#include <queue>

namespace Microsoft::React {

class WinRTWebSocketResource : public IWebSocketResource, public std::enable_shared_from_this<WinRTWebSocketResource> {
  winrt::Windows::Foundation::Uri m_uri;
  winrt::Windows::Networking::Sockets::IMessageWebSocket m_socket;
  // TODO: Use or remove.
  winrt::Windows::Networking::Sockets::IMessageWebSocket::MessageReceived_revoker m_revoker;
  winrt::Windows::Storage::Streams::IDataWriter m_writer;

  Mso::DispatchQueue m_dispatchQueue;
  Mso::ManualResetEvent m_closePerformed;
  std::atomic_bool m_connectRequested;
  std::promise<void> m_connectPerformedPromise;
  winrt::handle m_connectPerformed{
      CreateEvent(/*attributes*/ nullptr, /*manual reset*/ true, /*state*/ false, /*name*/ nullptr)};
  ReadyState m_readyState{ReadyState::Connecting};

  CloseCode m_closeCode{CloseCode::Normal};
  std::string m_closeReason;
  std::queue<std::pair<std::string, bool>> m_writeQueue;
  std::mutex m_writeQueueMutex;

  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string &, bool)> m_readHandler;
  std::function<void(CloseCode, const std::string &)> m_closeHandler;
  std::function<void(Error &&)> m_errorHandler;

  WinRTWebSocketResource(
      winrt::Windows::Networking::Sockets::IMessageWebSocket &&socket,
      winrt::Windows::Foundation::Uri &&uri,
      std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExeptions) noexcept;

  winrt::Windows::Foundation::IAsyncAction PerformConnect() noexcept;
  winrt::fire_and_forget PerformPing() noexcept;
  winrt::fire_and_forget PerformWrite(std::string &&message, bool isBinary) noexcept;
  winrt::fire_and_forget PerformClose() noexcept;

  void OnMessageReceived(
      winrt::Windows::Networking::Sockets::IWebSocket const &sender,
      winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs const &args);
  void Synchronize() noexcept;

 public:
  WinRTWebSocketResource(
      winrt::Windows::Networking::Sockets::IMessageWebSocket &&socket,
      winrt::Windows::Storage::Streams::IDataWriter &&writer,
      winrt::Windows::Foundation::Uri &&uri,
      std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>
          &&certExeptions) noexcept;

  WinRTWebSocketResource(
      const std::string &urlString,
      std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>
          &&certExeptions) noexcept;

  ~WinRTWebSocketResource() noexcept override;

#pragma region IWebSocketResource

  /// <summary>
  /// <see cref="IWebSocketResource::Connect" />
  /// </summary>
  void Connect(const Protocols &protocols, const Options &options) noexcept override;

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
