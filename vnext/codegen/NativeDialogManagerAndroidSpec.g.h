
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

REACT_STRUCT(DialogManagerAndroidSpec_DialogOptions)
struct DialogManagerAndroidSpec_DialogOptions {
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
    REACT_FIELD(items)
    std::optional<std::vector<std::string>> items;
    REACT_FIELD(cancelable)
    std::optional<bool> cancelable;
};

REACT_STRUCT(DialogManagerAndroidSpec_Constants)
struct DialogManagerAndroidSpec_Constants {
    REACT_FIELD(buttonClicked)
    std::string buttonClicked;
    REACT_FIELD(dismissed)
    std::string dismissed;
    REACT_FIELD(buttonPositive)
    double buttonPositive;
    REACT_FIELD(buttonNegative)
    double buttonNegative;
    REACT_FIELD(buttonNeutral)
    double buttonNeutral;
};

struct DialogManagerAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<DialogManagerAndroidSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(DialogManagerAndroidSpec_DialogOptions, Callback<std::string>, Callback<std::string, double>) noexcept>{0, L"showAlert"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, DialogManagerAndroidSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, DialogManagerAndroidSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "DialogManagerAndroidSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) DialogManagerAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static DialogManagerAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showAlert",
          "    REACT_METHOD(showAlert) void showAlert(DialogManagerAndroidSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, double)> const & onAction) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showAlert) static void showAlert(DialogManagerAndroidSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, double)> const & onAction) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
