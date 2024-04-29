// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSExecutor.h>
#include <functional>
#include <memory>
#include <string>

namespace facebook {
namespace react {

using JSECreator =
    std::function<std::unique_ptr<JSExecutor>(std::shared_ptr<ExecutorDelegate>, std::shared_ptr<MessageQueueThread>)>;

struct DevSettings;

struct IDevSupportManager {
  virtual JSECreator LoadJavaScriptInProxyMode(const DevSettings &settings, std::function<void()> &&errorCallback) = 0;
  virtual void StartPollingLiveReload(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort,
      std::function<void()> onChangeCallback) = 0;
  virtual void StopPollingLiveReload() = 0;

  virtual void EnsureHermesInspector(
      const std::string &packagerHost,
      const uint16_t packagerPort,
      const std::string &bundleAppId) noexcept = 0;
  virtual void UpdateBundleStatus(bool isLastDownloadSuccess, int64_t updateTimestamp) noexcept = 0;
};

std::shared_ptr<IDevSupportManager> CreateDevSupportManager();

} // namespace react
} // namespace facebook
