
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

struct DialogManagerWindowsSpec_DialogOptions {
    std::optional<std::string> title;
    std::optional<std::string> message;
    std::optional<std::string> buttonPositive;
    std::optional<std::string> buttonNegative;
    std::optional<std::string> buttonNeutral;
    std::optional<std::vector<std::string>> items;
    std::optional<bool> cancelable;
    std::optional<int> defaultButton;
    std::optional<int> rootTag;
};

struct DialogManagerWindowsSpec_Constants {
    std::string buttonClicked;
    std::string dismissed;
    int buttonPositive;
    int buttonNegative;
    int buttonNeutral;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DialogManagerWindowsSpec_DialogOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"title", &DialogManagerWindowsSpec_DialogOptions::title},
        {L"message", &DialogManagerWindowsSpec_DialogOptions::message},
        {L"buttonPositive", &DialogManagerWindowsSpec_DialogOptions::buttonPositive},
        {L"buttonNegative", &DialogManagerWindowsSpec_DialogOptions::buttonNegative},
        {L"buttonNeutral", &DialogManagerWindowsSpec_DialogOptions::buttonNeutral},
        {L"items", &DialogManagerWindowsSpec_DialogOptions::items},
        {L"cancelable", &DialogManagerWindowsSpec_DialogOptions::cancelable},
        {L"defaultButton", &DialogManagerWindowsSpec_DialogOptions::defaultButton},
        {L"rootTag", &DialogManagerWindowsSpec_DialogOptions::rootTag},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DialogManagerWindowsSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"buttonClicked", &DialogManagerWindowsSpec_Constants::buttonClicked},
        {L"dismissed", &DialogManagerWindowsSpec_Constants::dismissed},
        {L"buttonPositive", &DialogManagerWindowsSpec_Constants::buttonPositive},
        {L"buttonNegative", &DialogManagerWindowsSpec_Constants::buttonNegative},
        {L"buttonNeutral", &DialogManagerWindowsSpec_Constants::buttonNeutral},
    };
    return fieldMap;
}

struct DialogManagerWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<DialogManagerWindowsSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(DialogManagerWindowsSpec_DialogOptions, Callback<std::string>, Callback<std::string, int>) noexcept>{0, L"showAlert"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, DialogManagerWindowsSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, DialogManagerWindowsSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "DialogManagerWindowsSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) DialogManagerWindowsSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static DialogManagerWindowsSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showAlert",
          "    REACT_METHOD(showAlert) void showAlert(DialogManagerWindowsSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, int)> const & onAction) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(showAlert) static void showAlert(DialogManagerWindowsSpec_DialogOptions && config, std::function<void(std::string)> const & onError, std::function<void(std::string, int)> const & onAction) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
