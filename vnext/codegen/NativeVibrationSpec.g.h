
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct VibrationSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double) noexcept>{0, L"vibrate"},
      Method<void(JSValueArray,double) noexcept>{1, L"vibrateByPattern"},
      Method<void() noexcept>{2, L"cancel"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, VibrationSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "vibrate",
          "    REACT_METHOD(vibrate) void vibrate(double pattern) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "vibrateByPattern",
          "    REACT_METHOD(vibrateByPattern) void vibrateByPattern(React::JSValueArray && pattern, double repeat) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "cancel",
          "    REACT_METHOD(cancel) void cancel() noexcept { /* implementation */ }}");
  }
};

} // namespace Microsoft::ReactNativeSpecs
