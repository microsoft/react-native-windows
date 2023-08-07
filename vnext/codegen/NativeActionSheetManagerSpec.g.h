
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

struct ActionSheetManagerSpec_showActionSheetWithOptions_options {
    std::optional<std::string> title;
    std::optional<std::string> message;
    std::optional<std::vector<std::string>> options;
    std::optional<std::vector<double>> destructiveButtonIndices;
    std::optional<double> cancelButtonIndex;
    std::optional<double> anchor;
    std::optional<double> tintColor;
    std::optional<double> cancelButtonTintColor;
    std::optional<std::string> userInterfaceStyle;
    std::optional<std::vector<double>> disabledButtonIndices;
};

struct ActionSheetManagerSpec_showShareActionSheetWithOptions_options {
    std::optional<std::string> message;
    std::optional<std::string> url;
    std::optional<std::string> subject;
    std::optional<double> anchor;
    std::optional<double> tintColor;
    std::optional<double> cancelButtonTintColor;
    std::optional<std::vector<std::string>> excludedActivityTypes;
    std::optional<std::string> userInterfaceStyle;
};

struct ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error {
    std::string domain;
    std::string code;
    std::optional<::React::JSValue> userInfo;
    std::string message;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ActionSheetManagerSpec_showActionSheetWithOptions_options*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"title", &ActionSheetManagerSpec_showActionSheetWithOptions_options::title},
        {L"message", &ActionSheetManagerSpec_showActionSheetWithOptions_options::message},
        {L"options", &ActionSheetManagerSpec_showActionSheetWithOptions_options::options},
        {L"destructiveButtonIndices", &ActionSheetManagerSpec_showActionSheetWithOptions_options::destructiveButtonIndices},
        {L"cancelButtonIndex", &ActionSheetManagerSpec_showActionSheetWithOptions_options::cancelButtonIndex},
        {L"anchor", &ActionSheetManagerSpec_showActionSheetWithOptions_options::anchor},
        {L"tintColor", &ActionSheetManagerSpec_showActionSheetWithOptions_options::tintColor},
        {L"cancelButtonTintColor", &ActionSheetManagerSpec_showActionSheetWithOptions_options::cancelButtonTintColor},
        {L"userInterfaceStyle", &ActionSheetManagerSpec_showActionSheetWithOptions_options::userInterfaceStyle},
        {L"disabledButtonIndices", &ActionSheetManagerSpec_showActionSheetWithOptions_options::disabledButtonIndices},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ActionSheetManagerSpec_showShareActionSheetWithOptions_options*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"message", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::message},
        {L"url", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::url},
        {L"subject", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::subject},
        {L"anchor", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::anchor},
        {L"tintColor", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::tintColor},
        {L"cancelButtonTintColor", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::cancelButtonTintColor},
        {L"excludedActivityTypes", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::excludedActivityTypes},
        {L"userInterfaceStyle", &ActionSheetManagerSpec_showShareActionSheetWithOptions_options::userInterfaceStyle},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"domain", &ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error::domain},
        {L"code", &ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error::code},
        {L"userInfo", &ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error::userInfo},
        {L"message", &ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error::message},
    };
    return fieldMap;
}

struct ActionSheetManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(ActionSheetManagerSpec_showActionSheetWithOptions_options, Callback<double>) noexcept>{0, L"showActionSheetWithOptions"},
      Method<void(ActionSheetManagerSpec_showShareActionSheetWithOptions_options, Callback<ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error>, Callback<bool, std::optional<std::string>>) noexcept>{1, L"showShareActionSheetWithOptions"},
      Method<void() noexcept>{2, L"dismissActionSheet"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ActionSheetManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "showActionSheetWithOptions",
          "    REACT_METHOD(showActionSheetWithOptions) void showActionSheetWithOptions(ActionSheetManagerSpec_showActionSheetWithOptions_options && options, std::function<void(double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(showActionSheetWithOptions) static void showActionSheetWithOptions(ActionSheetManagerSpec_showActionSheetWithOptions_options && options, std::function<void(double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "showShareActionSheetWithOptions",
          "    REACT_METHOD(showShareActionSheetWithOptions) void showShareActionSheetWithOptions(ActionSheetManagerSpec_showShareActionSheetWithOptions_options && options, std::function<void(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error const &)> const & failureCallback, std::function<void(bool, std::optional<std::string>)> const & successCallback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(showShareActionSheetWithOptions) static void showShareActionSheetWithOptions(ActionSheetManagerSpec_showShareActionSheetWithOptions_options && options, std::function<void(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error const &)> const & failureCallback, std::function<void(bool, std::optional<std::string>)> const & successCallback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "dismissActionSheet",
          "    REACT_METHOD(dismissActionSheet) void dismissActionSheet() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(dismissActionSheet) static void dismissActionSheet() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
