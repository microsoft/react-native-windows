// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ReactInspectorPackagerConnectionDelegate.h"

#include <Networking/WinRTWebSocketResource.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Microsoft.ReactNative.h>
#include "ReactInspectorThread.h"

namespace Microsoft::ReactNative {

namespace {

class ReactInspectorWebSocket : public facebook::react::jsinspector_modern::IWebSocket {
 public:
  ReactInspectorWebSocket(
      std::string const &url,
      std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate);
  void send(std::string_view message) override;
  ~ReactInspectorWebSocket() override;

 private:
  std::shared_ptr<Microsoft::React::Networking::WinRTWebSocketResource> m_packagerWebSocketConnection;
  std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> m_weakDelegate;
  std::atomic<bool> m_didConnect{false};
};

ReactInspectorWebSocket::ReactInspectorWebSocket(
    std::string const &url,
    std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate)
    : m_weakDelegate{delegate} {
  std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;

  m_packagerWebSocketConnection =
      std::make_shared<Microsoft::React::Networking::WinRTWebSocketResource>(std::move(certExceptions));

  m_packagerWebSocketConnection->SetOnConnect([this, delegate]() {
    ReactInspectorThread::Instance().InvokeElsePost([this, delegate]() {
      m_didConnect = true;
      if (const auto strongDelegate = delegate.lock()) {
        strongDelegate->didOpen();
      }
    });
  });
  m_packagerWebSocketConnection->SetOnMessage([delegate](auto &&, const std::string &message, bool isBinary) {
    ReactInspectorThread::Instance().InvokeElsePost([delegate, message]() {
      if (const auto strongDelegate = delegate.lock()) {
        strongDelegate->didReceiveMessage(message);
      }
    });
  });
  m_packagerWebSocketConnection->SetOnError(
      [delegate](const Microsoft::React::Networking::IWebSocketResource::Error &error) {
        ReactInspectorThread::Instance().InvokeElsePost([delegate, error]() {
          if (const auto strongDelegate = delegate.lock()) {
            strongDelegate->didFailWithError(std::nullopt, error.Message);
          }
        });
      });
  m_packagerWebSocketConnection->SetOnClose([this, delegate](auto &&...) {
    ReactInspectorThread::Instance().InvokeElsePost([this, delegate]() {
      // Only call didClose() if we successfully connected first
      // This prevents didClose() from being called during failed connection attempts
      if (m_didConnect) {
        if (const auto strongDelegate = delegate.lock()) {
          strongDelegate->didClose();
        }
      }
    });
  });

  Microsoft::React::Networking::IWebSocketResource::Protocols protocols;
  Microsoft::React::Networking::IWebSocketResource::Options options;
  m_packagerWebSocketConnection->Connect(std::string{url}, protocols, options);
}

void ReactInspectorWebSocket::send(std::string_view message) {
  m_packagerWebSocketConnection->Send(std::string{message});
}

ReactInspectorWebSocket::~ReactInspectorWebSocket() {
  // Don't close WebSocket during shutdown - the OS will clean it up
  // Attempting to close async during DLL unload causes thread pool failures
  // The connection will be terminated when the process exits anyway
}

} // namespace

std::unique_ptr<facebook::react::jsinspector_modern::IWebSocket>
ReactInspectorPackagerConnectionDelegate::connectWebSocket(
    const std::string &url,
    std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate) {
  return std::make_unique<ReactInspectorWebSocket>(url, delegate);
}

winrt::fire_and_forget RunWithDelayAsync(std::function<void(void)> callback, std::chrono::milliseconds delayMs) {
  co_await winrt::resume_after(delayMs);
  ReactInspectorThread::Instance().InvokeElsePost([callback]() { callback(); });
}

void ReactInspectorPackagerConnectionDelegate::scheduleCallback(
    std::function<void(void)> callback,
    std::chrono::milliseconds delayMs) {
  RunWithDelayAsync(callback, delayMs);
}

} // namespace Microsoft::ReactNative
