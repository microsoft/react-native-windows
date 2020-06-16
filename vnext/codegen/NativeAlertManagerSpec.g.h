
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

struct AlertManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(React::JSValueObject, Callback<React::JSValue>) noexcept>{0, L"alertWithArgs"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AlertManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "alertWithArgs",
          "    REACT_METHOD(alertWithArgs) void alertWithArgs(React::JSValueObject && args, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(alertWithArgs) static void alertWithArgs(React::JSValueObject && args, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
