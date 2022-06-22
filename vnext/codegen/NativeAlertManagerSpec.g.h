
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

REACT_STRUCT(AlertManagerSpec_Args)
struct AlertManagerSpec_Args {
    REACT_FIELD(title)
    std::optional<std::string> title;
    REACT_FIELD(message)
    std::optional<std::string> message;
    REACT_FIELD(buttons)
    std::optional<std::vector<::React::JSValue>> buttons;
    REACT_FIELD(type)
    std::optional<std::string> type;
    REACT_FIELD(defaultValue)
    std::optional<std::string> defaultValue;
    REACT_FIELD(cancelButtonKey)
    std::optional<std::string> cancelButtonKey;
    REACT_FIELD(destructiveButtonKey)
    std::optional<std::string> destructiveButtonKey;
    REACT_FIELD(keyboardType)
    std::optional<std::string> keyboardType;
    REACT_FIELD(userInterfaceStyle)
    std::optional<std::string> userInterfaceStyle;
};

struct AlertManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(AlertManagerSpec_Args, Callback<double, std::string>) noexcept>{0, L"alertWithArgs"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AlertManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "alertWithArgs",
          "    REACT_METHOD(alertWithArgs) void alertWithArgs(AlertManagerSpec_Args && args, std::function<void(double, std::string)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(alertWithArgs) static void alertWithArgs(AlertManagerSpec_Args && args, std::function<void(double, std::string)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
