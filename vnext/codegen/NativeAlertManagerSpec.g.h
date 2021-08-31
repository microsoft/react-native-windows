
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
  struct Args {
      std::optional<std::string> title;
      std::optional<std::string> message;
      std::optional<React::JSValueArray> buttons;
      std::optional<std::string> type;
      std::optional<std::string> defaultValue;
      std::optional<std::string> cancelButtonKey;
      std::optional<std::string> destructiveButtonKey;
      std::optional<std::string> keyboardType;
  };

  static constexpr auto methods = std::tuple{
      Method<void(Args, Callback<React::JSValue>) noexcept>{0, L"alertWithArgs"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AlertManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "alertWithArgs",
          "    REACT_METHOD(alertWithArgs) void alertWithArgs(Args && args, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(alertWithArgs) static void alertWithArgs(Args && args, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
