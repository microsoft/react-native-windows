// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

namespace Microsoft::ReactNative {

class HermesSamplingProfiler final {
 public:
  static winrt::fire_and_forget Start() noexcept;
  static std::future<std::string> Stop() noexcept;
  static std::string GetLastTraceFilePath() noexcept;
  static bool IsStarted() noexcept;

 private:
  static bool s_isStarted;
  static std::string s_lastTraceFilePath;
};

} // namespace Microsoft::ReactNative
