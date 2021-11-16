
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

struct ClipboardSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Promise<::React::JSValue>) noexcept>{0, L"getString"},
      Method<void(std::string) noexcept>{1, L"setString"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ClipboardSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getString",
          "    REACT_METHOD(getString) void getString(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getString) static void getString(::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setString",
          "    REACT_METHOD(setString) void setString(std::string content) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setString) static void setString(std::string content) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
