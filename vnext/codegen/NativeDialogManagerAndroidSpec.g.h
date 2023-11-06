
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct DialogManagerAndroidSpec_DialogOptions {
    std::optional<std::string> title;
    std::optional<std::string> message;
    std::optional<std::string> buttonPositive;
    std::optional<std::string> buttonNegative;
    std::optional<std::string> buttonNeutral;
    std::optional<std::vector<std::string>> items;
    std::optional<bool> cancelable;
};

struct DialogManagerAndroidSpec_Constants {
    std::string buttonClicked;
    std::string dismissed;
    double buttonPositive;
    double buttonNegative;
    double buttonNeutral;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DialogManagerAndroidSpec_DialogOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"title", &DialogManagerAndroidSpec_DialogOptions::title},
        {L"message", &DialogManagerAndroidSpec_DialogOptions::message},
        {L"buttonPositive", &DialogManagerAndroidSpec_DialogOptions::buttonPositive},
        {L"buttonNegative", &DialogManagerAndroidSpec_DialogOptions::buttonNegative},
        {L"buttonNeutral", &DialogManagerAndroidSpec_DialogOptions::buttonNeutral},
        {L"items", &DialogManagerAndroidSpec_DialogOptions::items},
        {L"cancelable", &DialogManagerAndroidSpec_DialogOptions::cancelable},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DialogManagerAndroidSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"buttonClicked", &DialogManagerAndroidSpec_Constants::buttonClicked},
        {L"dismissed", &DialogManagerAndroidSpec_Constants::dismissed},
        {L"buttonPositive", &DialogManagerAndroidSpec_Constants::buttonPositive},
        {L"buttonNegative", &DialogManagerAndroidSpec_Constants::buttonNegative},
        {L"buttonNeutral", &DialogManagerAndroidSpec_Constants::buttonNeutral},
    };
    return fieldMap;
}

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
          "    REACT_METHOD(showAlert) void showAlert(DialogManagerAndroidSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, double)> const & onAction) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(showAlert) static void showAlert(DialogManagerAndroidSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, double)> const & onAction) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
