// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IDevSupportManager.h>

#include <DevServerHelper.h>

#include <winrt/Windows.Networking.Sockets.h>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <string>

#include <jsinspector-modern/InspectorPackagerConnection.h>

namespace facebook {
namespace react {
struct DevSettings;
}
} // namespace facebook

namespace Microsoft::ReactNative {

std::pair<std::string, bool> GetJavaScriptFromServer(
    const std::string &sourceBundleHost,
    const uint16_t sourceBundlePort,
    const std::string &jsBundleName,
    const std::string &platform,
    const std::string &bundleAppId,
    bool dev,
    bool hot,
    bool inlineSourceMap,
    uint32_t hermesBytecodeVersion);

class DevSupportManager final : public facebook::react::IDevSupportManager {
 public:
  DevSupportManager() = default;
  ~DevSupportManager();

  virtual facebook::react::JSECreator LoadJavaScriptInProxyMode(
      const facebook::react::DevSettings &settings,
      std::function<void()> &&errorCallback) override;
  virtual void StartPollingLiveReload(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort,
      std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;
  virtual void OpenDevTools(const std::string &bundleAppId) override;

  virtual void EnsureInspectorPackagerConnection(
      const std::string &packagerHost,
      const uint16_t packagerPort,
      const std::string &bundleAppId) noexcept override;

 private:
  std::atomic_bool m_cancellation_token;

  std::unique_ptr<facebook::react::jsinspector_modern::InspectorPackagerConnection> m_inspectorPackagerConnection;
};

} // namespace Microsoft::ReactNative
