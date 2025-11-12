// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <jsinspector-modern/InspectorPackagerConnection.h>

namespace Microsoft::ReactNative {

class ReactInspectorPackagerConnectionDelegate final
    : public facebook::react::jsinspector_modern::InspectorPackagerConnectionDelegate {
 public: // InspectorPackagerConnectionDelegate implementation
  std::unique_ptr<facebook::react::jsinspector_modern::IWebSocket> connectWebSocket(
      const std::string &url,
      std::weak_ptr<facebook::react::jsinspector_modern::IWebSocketDelegate> delegate) override;

  void scheduleCallback(std::function<void(void)> callback, std::chrono::milliseconds delayMs) override;
};

} // namespace Microsoft::ReactNative
