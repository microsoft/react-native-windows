// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "Utils.h"

namespace facebook {
namespace react {

class DevServerHelper {
 public:
  DevServerHelper() = default;

  static std::string get_WebsocketProxyUrl(const std::string &sourceBundleHost, const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        WebsocketProxyUrlFormat, GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str());
  }

  static std::string get_LaunchDevToolsCommandUrl(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        LaunchDevToolsCommandUrlFormat, GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str());
  }

  static std::string get_BundleUrl(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort,
      const std::string &jsbundle,
      const std::string &platform,
      const std::string &bundleAppId,
      bool dev,
      bool hot,
      bool inlineSourceMap,
      const uint32_t hermesBytecodeVersion) {
    std::string hermesBytecodeVersionQuery;
    if (hermesBytecodeVersion > 0) {
      static constexpr const char HermesBytecodeVersionQueryFormat[] = "&runtimeBytecodeVersion=%d";
      hermesBytecodeVersionQuery =
          Microsoft::React::FormatString(HermesBytecodeVersionQueryFormat, hermesBytecodeVersion);
    }

    std::string appIdQuery;
    if (bundleAppId.size() > 0) {
      static constexpr const char AppIdQueryFormat[] = "&app=%s";
      appIdQuery = Microsoft::React::FormatString(AppIdQueryFormat, bundleAppId.c_str());
    }

    return Microsoft::React::FormatString(
        BundleUrlFormat,
        GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str(),
        jsbundle.c_str(),
        platform.c_str(),
        dev ? "true" : "false",
        hot ? "true" : "false",
        inlineSourceMap ? "true" : "false",
        hermesBytecodeVersionQuery.c_str(),
        appIdQuery.c_str());
  }

  static std::string get_OnChangeEndpointUrl(const std::string &sourceBundleHost, const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        OnChangeEndpointUrlFormat, GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str());
  }

  static std::string get_PackagerConnectionUrl(const std::string &sourceBundleHost, const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        PackagerConnectionUrlFormat, GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str());
  }

  static std::string get_PackagerOpenStackFrameUrl(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        PackagerOpenStackFrameUrlFormat, GetDeviceLocalHost(sourceBundleHost, sourceBundlePort).c_str());
  }

  static std::string get_InspectorDeviceUrl(
      const std::string &packagerHost,
      const uint16_t packagerPort,
      const std::string &deviceName,
      const std::string &packageName,
      const std::string &deviceId) {
    return Microsoft::React::FormatString(
        InspectorDeviceUrlFormat,
        GetDeviceLocalHost(packagerHost, packagerPort).c_str(),
        deviceName.c_str(),
        packageName.c_str(),
        deviceId.c_str());
  }

  static std::string
  get_OpenDebuggerUrl(const std::string &packagerHost, const uint16_t packagerPort, const std::string &deviceId) {
    return Microsoft::React::FormatString(
        OpenDebuggerUrlFormat, GetDeviceLocalHost(packagerHost, packagerPort).c_str(), deviceId.c_str());
  }

  static constexpr const char DefaultPackagerHost[] = "localhost";
  static const uint16_t DefaultPackagerPort = 8081;

 private:
  static std::string GetDeviceLocalHost(const std::string &sourceBundleHost, const uint16_t sourceBundlePort) {
    return Microsoft::React::FormatString(
        DeviceLocalHostFormat,
        sourceBundleHost.empty() ? DefaultPackagerHost : sourceBundleHost.c_str(),
        sourceBundlePort ? sourceBundlePort : DefaultPackagerPort);
  }

  static constexpr const char DeviceLocalHostFormat[] = "%s:%d";
  static constexpr const char BundleUrlFormat[] =
      "http://%s/%s.bundle?platform=%s&dev=%s&hot=%s&inlineSourceMap=%s%s%s";
  static constexpr const char SourceMapUrlFormat[] = "http://%s/%s.map?platform=%s&dev=%s&hot=%s";
  static constexpr const char LaunchDevToolsCommandUrlFormat[] = "http://%s/launch-js-devtools";
  static constexpr const char OnChangeEndpointUrlFormat[] = "http://%s/onchange";
  static constexpr const char WebsocketProxyUrlFormat[] = "ws://%s/debugger-proxy?role=client";
  static constexpr const char PackagerConnectionUrlFormat[] = "ws://%s/message";
  static constexpr const char PackagerStatusUrlFormat[] = "http://%s/status";
  static constexpr const char PackagerOpenStackFrameUrlFormat[] = "https://%s/open-stack-frame";
  static constexpr const char InspectorDeviceUrlFormat[] = "ws://%s/inspector/device?name=%s&app=%s&device=%s";
  static constexpr const char OpenDebuggerUrlFormat[] = "http://%s/open-debugger?device=%s";

  static constexpr const char PackagerOkStatus[] = "packager-status:running";
  const int LongPollFailureDelayMs = 5000;
};

} // namespace react
} // namespace facebook
