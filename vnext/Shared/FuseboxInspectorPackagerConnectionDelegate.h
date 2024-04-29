// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Networking/WinRTWebSocketResource.h>
#include <dispatchQueue/dispatchQueue.h>
#include <jsinspector-modern/InspectorPackagerConnection.h>

namespace Microsoft::ReactNative {

class FuseboxInspectorPackagerConnectionDelegate final
    : public facebook::react::jsinspector_modern::InspectorPackagerConnectionDelegate {
  class WebSocket : public facebook::react::jsinspector_modern::IWebSocket {
   public:
    WebSocket(std::string const &url, std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate);
    virtual void send(std::string_view message) override;
    virtual ~WebSocket() override;

   private:
    std::shared_ptr<Microsoft::React::Networking::WinRTWebSocketResource> m_packagerWebSocketConnection;
    std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> m_weakDelegate;
  };

 public:
  virtual std::unique_ptr<facebook::react::jsinspector_modern::IWebSocket> connectWebSocket(
      const std::string &url,
      std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate) override;

  virtual void scheduleCallback(std::function<void(void)> callback, std::chrono::milliseconds delayMs) override;
};
} // namespace Microsoft::ReactNative
