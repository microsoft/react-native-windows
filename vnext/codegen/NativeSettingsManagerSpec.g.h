
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

struct SettingsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(React::JSValueObject) noexcept>{0, L"setValues"},
      Method<void(React::JSValueArray) noexcept>{1, L"deleteValues"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SettingsManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setValues",
          "    REACT_METHOD(setValues) void setValues(React::JSValueObject && values) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setValues) static void setValues(React::JSValueObject && values) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "deleteValues",
          "    REACT_METHOD(deleteValues) void deleteValues(React::JSValueArray && values) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(deleteValues) static void deleteValues(React::JSValueArray && values) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
