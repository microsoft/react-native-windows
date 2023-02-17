// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactHost/React.h>
#include <atomic>
#include <future>
#include <string>

namespace Microsoft::ReactNative {

class HermesSamplingProfiler final {
 public:
  static winrt::fire_and_forget Start(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept;
  static std::future<std::string> Stop(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept;
  static std::string GetLastTraceFilePath() noexcept;
  static bool IsStarted() noexcept;

 private:
  static std::atomic_bool s_isStarted;
  static std::string s_lastTraceFilePath;
};

} // namespace Microsoft::ReactNative
