// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSExecutor.h>

namespace facebook {
namespace react {

using JSECreator =
    std::function<std::unique_ptr<JSExecutor>(std::shared_ptr<ExecutorDelegate>, std::shared_ptr<MessageQueueThread>)>;

class DevServerHelper {
 public:
  DevServerHelper() = default;

  static std::string get_WebsocketProxyUrl(const std::string &debugHost) {
    return string_format(WebsocketProxyUrlFormat, GetDeviceLocalHost(debugHost).c_str());
  }

  static std::string get_LaunchDevToolsCommandUrl(const std::string &debugHost) {
    return string_format(LaunchDevToolsCommandUrlFormat, GetDeviceLocalHost(debugHost).c_str());
  }

  static std::string get_BundleUrl(
      const std::string &debugHost,
      const std::string &jsbundle,
      const std::string &platform,
      bool dev,
      bool hot,
      bool inlineSourceMap) {
    return string_format(
        BundleUrlFormat,
        GetDeviceLocalHost(debugHost).c_str(),
        jsbundle.c_str(),
        platform.c_str(),
        dev ? "true" : "false",
        hot ? "true" : "false",
        inlineSourceMap ? "true" : "false");
  }

  static std::string get_OnChangeEndpointUrl(const std::string &debugHost) {
    return string_format(OnChangeEndpointUrlFormat, GetDeviceLocalHost(debugHost).c_str());
  }

 private:
  static std::string GetDeviceLocalHost(const std::string &debugHost) {
    return debugHost.empty() ? s_DeviceLocalhost : debugHost;
  }

  static constexpr const char s_DeviceLocalhost[] = "localhost:8081";
  static constexpr const char BundleUrlFormat[] = "http://%s/%s.bundle?platform=%s&dev=%s&hot=%s&inlineSourceMap=%s";
  static constexpr const char SourceMapUrlFormat[] = "http://%s/%s.map?platform=%s&dev=%s&hot=%s";
  static constexpr const char LaunchDevToolsCommandUrlFormat[] = "http://%s/launch-js-devtools";
  static constexpr const char OnChangeEndpointUrlFormat[] = "http://%s/onchange";
  static constexpr const char WebsocketProxyUrlFormat[] = "ws://%s/debugger-proxy?role=client";
  static constexpr const char PackagerStatusUrlFormat[] = "http://%s/status";
  static constexpr const char PackagerOkStatus[] = "packager-status:running";
  const int LongPollFailureDelayMs = 5000;

  template <typename... Args>
  static std::string string_format(const char *format, Args... args) {
    size_t size = snprintf(nullptr, 0, format, args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format, args...);
    return std::string(buf.get(), buf.get() + size - 1);
  }
};

} // namespace react
} // namespace facebook
