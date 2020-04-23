
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

struct JSDevSupportSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(JSValueObject) noexcept>{0, L"onSuccess"},
      Method<void(double, std::string) noexcept>{1, L"onFailure"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, JSDevSupportSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "onSuccess",
          "    REACT_METHOD(onSuccess) void onSuccess(React::JSValueObject && data) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onSuccess) static void onSuccess(React::JSValueObject && data) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "onFailure",
          "    REACT_METHOD(onFailure) void onFailure(double errorCode, std::string error) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onFailure) static void onFailure(double errorCode, std::string error) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
