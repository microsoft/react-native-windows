#pragma once

#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

struct LinkingHelper {
  static winrt::hstring InitialUrl(IReactPropertyBag const &properties) noexcept;
  static void SetInitialUrl(IReactPropertyBag const &properties, winrt::hstring const &url) noexcept;
};

} // namespace winrt::Microsoft::ReactNative
