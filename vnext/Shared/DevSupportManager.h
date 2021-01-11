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
    bool inlineSourceMap);

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

 private:
  std::atomic_bool m_cancellation_token;
};

} // namespace Microsoft::ReactNative
