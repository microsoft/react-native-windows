// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DevServerHelper.h"

#include <functional>
#include <memory>
#include <string>

namespace facebook {
namespace react {

struct DevSettings;

struct IDevSupportManager {
  virtual JSECreator LoadJavaScriptInProxyMode(const DevSettings &settings) = 0;
#if !defined(OSS_RN)
  virtual JSECreator LoadJavaScriptInSandboxMode(
      const DevSettings &settings) = 0;
#endif
  virtual std::string GetJavaScriptFromServer(
      const std::string &debugHost,
      const std::string &jsBundleName,
      const std::string &platform) = 0;
  virtual void StartPollingLiveReload(
      const std::string &debugHost,
      std::function<void()> onChangeCallback) = 0;
  virtual void StopPollingLiveReload() = 0;
  virtual bool HasException() = 0;
};

std::shared_ptr<IDevSupportManager> CreateDevSupportManager();

} // namespace react
} // namespace facebook
