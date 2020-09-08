// Copyright (c) Microsoft Corporation. All rights reserved.
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

namespace react {
namespace uwp {

class DevSupportManager : public facebook::react::IDevSupportManager {
 public:
  DevSupportManager() = default;
  ~DevSupportManager();

  virtual facebook::react::JSECreator LoadJavaScriptInProxyMode(const facebook::react::DevSettings &settings) override;
  virtual std::string GetJavaScriptFromServer(
      const std::string &debugHost,
      const std::string &jsBundleName,
      const std::string &platform,
      bool inlineSourceMap) override;
  virtual void StartPollingLiveReload(const std::string &debugHost, std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;
  virtual bool HasException() override {
    return m_exceptionCaught;
  }

 private:
  void LaunchDevTools(const facebook::react::DevSettings &settings);
  std::future<void> CreatePackagerConnection(const facebook::react::DevSettings &settings);

 private:
  winrt::Windows::Networking::Sockets::MessageWebSocket m_ws{nullptr};
  winrt::Windows::Networking::Sockets::MessageWebSocket::MessageReceived_revoker m_wsMessageRevoker;
  bool m_exceptionCaught = false;
  std::atomic_bool m_cancellation_token;
};

} // namespace uwp
} // namespace react
