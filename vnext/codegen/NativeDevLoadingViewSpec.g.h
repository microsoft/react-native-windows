
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

struct DevLoadingViewSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, JSValueObject, JSValueObject) noexcept>{0, L"showMessage"},
      Method<void() noexcept>{1, L"hide"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DevLoadingViewSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showMessage",
          "    REACT_METHOD(showMessage) void showMessage(std::string message, React::JSValueObject && color, React::JSValueObject && backgroundColor) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showMessage) static void showMessage(std::string message, React::JSValueObject && color, React::JSValueObject && backgroundColor) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "hide",
          "    REACT_METHOD(hide) void hide() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(hide) static void hide() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
