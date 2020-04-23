
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

struct DeviceEventManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"invokeDefaultBackPressHandler"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DeviceEventManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "invokeDefaultBackPressHandler",
          "    REACT_METHOD(invokeDefaultBackPressHandler) void invokeDefaultBackPressHandler() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(invokeDefaultBackPressHandler) static void invokeDefaultBackPressHandler() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
