// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ppltasks.h>
#undef U

#include <IDevSupportManager.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/system/error_code.hpp>
#include <cxxreact/JSExecutor.h>

namespace facebook {
namespace react {

using JSECreator = std::function<std::unique_ptr<JSExecutor>(
    std::shared_ptr<ExecutorDelegate>,
    std::shared_ptr<MessageQueueThread>)>;

struct DevSettings;

class DevSupportManager : public IDevSupportManager {
 public:
  DevSupportManager();
  ~DevSupportManager();

  virtual JSECreator LoadJavaScriptInProxyMode(
      const DevSettings &settings) override;
#if !defined(OSS_RN)
  virtual JSECreator LoadJavaScriptInSandboxMode(
      const DevSettings &settings) override;
#endif
  virtual std::string GetJavaScriptFromServer(
      const std::string &debugHost,
      const std::string &jsBundleName,
      const std::string &platform) override;
  virtual void StartPollingLiveReload(
      const std::string &debugHost,
      std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;
  virtual bool HasException() override {
    return m_exceptionCaught;
  }

 private:
  Concurrency::task<void> LaunchDevToolsAsync(
      const std::string &debugHost,
      const Concurrency::cancellation_token &token =
          Concurrency::cancellation_token::none());

  bool m_exceptionCaught = false;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::resolver m_resolver;
  Concurrency::cancellation_token_source m_liveReloadCts;
};

} // namespace react
} // namespace facebook
