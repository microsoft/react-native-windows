// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IDevSupportManager.h>

#include <DevServerHelper.h>

#include <functional>
#include <memory>
#include <string>
#include <atomic>

namespace facebook { namespace react {
struct DevSettings;
}}

namespace react { namespace uwp {

class DevSupportManager : public facebook::react::IDevSupportManager
{
public:
  DevSupportManager() = default;
  ~DevSupportManager();

  virtual facebook::react::JSECreator LoadJavaScriptInProxyMode(const facebook::react::DevSettings& settings) override;
#if !defined(OSS_RN)
  virtual facebook::react::JSECreator LoadJavaScriptInSandboxMode(const facebook::react::DevSettings& settings) override;
#endif
  virtual std::string GetJavaScriptFromServer(const std::string& debugHost, const std::string& jsBundleName, const std::string& platform) override;
  virtual void StartPollingLiveReload(const std::string& debugHost, std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;
  virtual bool HasException() override { return m_exceptionCaught; }

private:
  void LaunchDevTools(const facebook::react::DevSettings& settings);

private:
  bool m_exceptionCaught = false;
  std::atomic_bool m_cancellation_token;
};

} }
