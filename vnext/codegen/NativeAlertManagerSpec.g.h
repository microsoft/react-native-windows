
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
      REACT_FIELD(title)
      std::optional<std::string> title;
      REACT_FIELD(message)
      std::optional<std::string> message;
      REACT_FIELD(buttons)
      std::optional<React::JSValueArray> buttons;
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

  INTERNAL_REACT_STRUCT_GETSTRUCTINFO(AlertManagerSpec::Args)

} // namespace Microsoft::ReactNativeSpecs
