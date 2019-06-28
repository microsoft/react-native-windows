// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IDevSupportManager.h>
#include "DevSupportManager.h"

namespace facebook { namespace react {

///
// Proxy IDevSupportManager implementation.
// Wraps actual DevSupportManager instantiation until public methods are used.
///
class LazyDevSupportManager : public IDevSupportManager
{
  std::unique_ptr<DevSupportManager> m_dsm;

  void EnsureDevSupportManager() noexcept;

public:
  #pragma region IDevSupportManager

  JSECreator LoadJavaScriptInProxyMode(const DevSettings& settings) override;
  #if !defined(OSS_RN)
  JSECreator LoadJavaScriptInSandboxMode(const DevSettings& settings) override;
  #endif
  std::string GetJavaScriptFromServer(const std::string& debugHost, const std::string& jsBundleName, const std::string& platform) override;
  void StartPollingLiveReload(const std::string& debugHost, std::function<void()> onChangeCallback) override;
  void StopPollingLiveReload() override;
  bool HasException() override;

  #pragma endregion IDevSupportManager
};

} } // namespace facebook::react
