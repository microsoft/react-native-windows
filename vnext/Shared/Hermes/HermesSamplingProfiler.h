// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactHost/React.h>
#include <winrt/Windows.Foundation.h>
#include <atomic>
#include <string>

namespace Microsoft::ReactNative {

class HermesSamplingProfiler final {
 public:
  static winrt::fire_and_forget Start(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept;
  static winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> Stop(
      Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept;
  static winrt::hstring GetLastTraceFilePath() noexcept;
  static bool IsStarted() noexcept;

 private:
  static std::atomic_bool s_isStarted;
  static winrt::hstring s_lastTraceFilePath;
};

} // namespace Microsoft::ReactNative
