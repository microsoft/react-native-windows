
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

struct RedBoxSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(JSValueObject, std::string) noexcept>{0, L"setExtraData"},
      Method<void() noexcept>{1, L"dismiss"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, RedBoxSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setExtraData",
          "    REACT_METHOD(setExtraData) void setExtraData(React::JSValueObject && extraData, std::string forIdentifier) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setExtraData) static void setExtraData(React::JSValueObject && extraData, std::string forIdentifier) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "dismiss",
          "    REACT_METHOD(dismiss) void dismiss() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismiss) static void dismiss() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
