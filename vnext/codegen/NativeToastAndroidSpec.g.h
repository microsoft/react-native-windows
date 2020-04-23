
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

struct ToastAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string,double) noexcept>{0, L"show"},
      Method<void(std::string,double,double) noexcept>{1, L"showWithGravity"},
      Method<void(std::string,double,double,double,double) noexcept>{2, L"showWithGravityAndOffset"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ToastAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "show",
          "    REACT_METHOD(show) void show(std::string message, double duration) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "showWithGravity",
          "    REACT_METHOD(showWithGravity) void showWithGravity(std::string message, double duration, double gravity) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "showWithGravityAndOffset",
          "    REACT_METHOD(showWithGravityAndOffset) void showWithGravityAndOffset(std::string message, double duration, double gravity, double xOffset, double yOffset) noexcept { /* implementation */ }}");
  }
};

} // namespace Microsoft::ReactNativeSpecs
