
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

struct DatePickerAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(::React::JSValue, Promise<::React::JSValue>) noexcept>{0, L"open"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DatePickerAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "open",
          "    REACT_METHOD(open) void open(::React::JSValue && options, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(open) static void open(::React::JSValue && options, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
