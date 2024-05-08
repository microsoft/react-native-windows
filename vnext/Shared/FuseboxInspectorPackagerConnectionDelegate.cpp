// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FuseboxInspectorPackagerConnectionDelegate.h"

#include <winrt/Microsoft.ReactNative.h>
#include "FuseboxInspectorThread.h"

namespace Microsoft::ReactNative {

FuseboxInspectorPackagerConnectionDelegate::WebSocket::WebSocket(
    std::string const &url,
    std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate)
    : m_weakDelegate{delegate} {
  std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;

  m_packagerWebSocketConnection =
      std::make_shared<Microsoft::React::Networking::WinRTWebSocketResource>(std::move(certExceptions));

  m_packagerWebSocketConnection->SetOnMessage([delegate](auto &&, const std::string &message, bool isBinary) {
    FuseboxInspectorThread::Instance().InvokeElsePost([delegate, message]() {
      if (const auto strongDelegate = delegate.lock()) {
        strongDelegate->didReceiveMessage(message);
      }
    });
  });
  m_packagerWebSocketConnection->SetOnError(
      [delegate](const Microsoft::React::Networking::IWebSocketResource::Error &error) {
        FuseboxInspectorThread::Instance().InvokeElsePost([delegate, error]() {
          if (const auto strongDelegate = delegate.lock()) {
            strongDelegate->didFailWithError(std::nullopt, error.Message);
          }
        });
      });
  m_packagerWebSocketConnection->SetOnClose([delegate](auto &&...) {
    FuseboxInspectorThread::Instance().InvokeElsePost([delegate]() {
      if (const auto strongDelegate = delegate.lock()) {
        strongDelegate->didClose();
      }
    });
  });

  Microsoft::React::Networking::IWebSocketResource::Protocols protocols;
  Microsoft::React::Networking::IWebSocketResource::Options options;
  m_packagerWebSocketConnection->Connect(std::string{url}, protocols, options);
}

void FuseboxInspectorPackagerConnectionDelegate::WebSocket::send(std::string_view message) {
  m_packagerWebSocketConnection->Send(std::string{message});
}

FuseboxInspectorPackagerConnectionDelegate::WebSocket::~WebSocket() {
  std::string reason{"Explicit close"};
  m_packagerWebSocketConnection->Close(
      Microsoft::React::Networking::WinRTWebSocketResource::CloseCode::GoingAway, reason);
}

std::unique_ptr<facebook::react::jsinspector_modern::IWebSocket>
FuseboxInspectorPackagerConnectionDelegate::connectWebSocket(
    const std::string &url,
    std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate) {
  return std::make_unique<WebSocket>(url, delegate);
}

winrt::fire_and_forget RunWithDelayAsync(std::function<void(void)> callback, std::chrono::milliseconds delayMs) {
  co_await winrt::resume_after(delayMs);
  FuseboxInspectorThread::Instance().InvokeElsePost([callback]() { callback(); });
}

void FuseboxInspectorPackagerConnectionDelegate::scheduleCallback(
    std::function<void(void)> callback,
    std::chrono::milliseconds delayMs) {
  RunWithDelayAsync(callback, delayMs);
}

} // namespace Microsoft::ReactNative
