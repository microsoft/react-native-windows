
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

REACT_STRUCT(DialogManagerWindowsSpec_DialogOptions)
struct DialogManagerWindowsSpec_DialogOptions {
    REACT_FIELD(title)
    std::optional<std::string> title;
    REACT_FIELD(message)
    std::optional<std::string> message;
    REACT_FIELD(buttonPositive)
    std::optional<std::string> buttonPositive;
    REACT_FIELD(buttonNegative)
    std::optional<std::string> buttonNegative;
    REACT_FIELD(buttonNeutral)
    std::optional<std::string> buttonNeutral;
    REACT_FIELD(cancelable)
    std::optional<bool> cancelable;
    REACT_FIELD(defaultButton)
    std::optional<int> defaultButton;
};

struct DialogManagerWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(DialogManagerWindowsSpec_DialogOptions, Callback<std::string>) noexcept>{0, L"showAlert"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DialogManagerWindowsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showAlert",
          "    REACT_METHOD(showAlert) void showAlert(DialogManagerWindowsSpec_DialogOptions && config, std::function<void(std::string)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showAlert) static void showAlert(DialogManagerWindowsSpec_DialogOptions && config, std::function<void(std::string)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
