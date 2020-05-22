// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ppltasks.h>
#undef U

#include <IDevSupportManager.h>
#include <cxxreact/JSExecutor.h>

namespace Microsoft::React {

using JSECreator = std::function<std::unique_ptr<facebook::react::JSExecutor>(
    std::shared_ptr<facebook::react::ExecutorDelegate>,
    std::shared_ptr<facebook::react::MessageQueueThread>)>;

class DevSupportManager : public facebook::react::IDevSupportManager {
 public:
  DevSupportManager();
  ~DevSupportManager();

  virtual facebook::react::JSECreator LoadJavaScriptInProxyMode(const facebook::react::DevSettings &settings) override;
  virtual std::string GetJavaScriptFromServer(
      const std::string &debugHost,
      const std::string &jsBundleName,
      const std::string &platform) override;
  virtual void StartPollingLiveReload(const std::string &debugHost, std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;
  virtual bool HasException() override {
    return m_exceptionCaught;
  }

 private:
  Concurrency::task<void> LaunchDevToolsAsync(
      const std::string &debugHost,
      const Concurrency::cancellation_token &token = Concurrency::cancellation_token::none());

  bool m_exceptionCaught = false;
  Concurrency::cancellation_token_source m_liveReloadCts;
};

} // namespace Microsoft::React
