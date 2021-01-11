
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

struct ActionSheetManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(React::JSValueObject, Callback<React::JSValue>) noexcept>{0, L"showActionSheetWithOptions"},
      Method<void(React::JSValueObject, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{1, L"showShareActionSheetWithOptions"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ActionSheetManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showActionSheetWithOptions",
          "    REACT_METHOD(showActionSheetWithOptions) void showActionSheetWithOptions(React::JSValueObject && options, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showActionSheetWithOptions) static void showActionSheetWithOptions(React::JSValueObject && options, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "showShareActionSheetWithOptions",
          "    REACT_METHOD(showShareActionSheetWithOptions) void showShareActionSheetWithOptions(React::JSValueObject && options, std::function<void(React::JSValue const &)> const & failureCallback, std::function<void(React::JSValue const &)> const & successCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showShareActionSheetWithOptions) static void showShareActionSheetWithOptions(React::JSValueObject && options, std::function<void(React::JSValue const &)> const & failureCallback, std::function<void(React::JSValue const &)> const & successCallback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
