
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

REACT_STRUCT(ActionSheetManagerSpec_showActionSheetWithOptions_options)
struct ActionSheetManagerSpec_showActionSheetWithOptions_options {
    REACT_FIELD(title)
    std::optional<std::string> title;
    REACT_FIELD(message)
    std::optional<std::string> message;
    REACT_FIELD(options)
    std::optional<std::vector<std::string>> options;
    REACT_FIELD(destructiveButtonIndices)
    std::optional<std::vector<double>> destructiveButtonIndices;
    REACT_FIELD(cancelButtonIndex)
    std::optional<double> cancelButtonIndex;
    REACT_FIELD(anchor)
    std::optional<double> anchor;
    REACT_FIELD(tintColor)
    std::optional<double> tintColor;
    REACT_FIELD(cancelButtonTintColor)
    std::optional<double> cancelButtonTintColor;
    REACT_FIELD(userInterfaceStyle)
    std::optional<std::string> userInterfaceStyle;
    REACT_FIELD(disabledButtonIndices)
    std::optional<std::vector<double>> disabledButtonIndices;
};

REACT_STRUCT(ActionSheetManagerSpec_showShareActionSheetWithOptions_options)
struct ActionSheetManagerSpec_showShareActionSheetWithOptions_options {
    REACT_FIELD(message)
    std::optional<std::string> message;
    REACT_FIELD(url)
    std::optional<std::string> url;
    REACT_FIELD(subject)
    std::optional<std::string> subject;
    REACT_FIELD(anchor)
    std::optional<double> anchor;
    REACT_FIELD(tintColor)
    std::optional<double> tintColor;
    REACT_FIELD(cancelButtonTintColor)
    std::optional<double> cancelButtonTintColor;
    REACT_FIELD(excludedActivityTypes)
    std::optional<std::vector<std::string>> excludedActivityTypes;
    REACT_FIELD(userInterfaceStyle)
    std::optional<std::string> userInterfaceStyle;
};

REACT_STRUCT(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error)
struct ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error {
    REACT_FIELD(domain)
    std::string domain;
    REACT_FIELD(code)
    std::string code;
    REACT_FIELD(userInfo)
    std::optional<::React::JSValue> userInfo;
    REACT_FIELD(message)
    std::string message;
};

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
          "    REACT_METHOD(showActionSheetWithOptions) void showActionSheetWithOptions(ActionSheetManagerSpec_showActionSheetWithOptions_options && options, std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showActionSheetWithOptions) static void showActionSheetWithOptions(ActionSheetManagerSpec_showActionSheetWithOptions_options && options, std::function<void(double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "showShareActionSheetWithOptions",
          "    REACT_METHOD(showShareActionSheetWithOptions) void showShareActionSheetWithOptions(ActionSheetManagerSpec_showShareActionSheetWithOptions_options && options, std::function<void(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error const &)> const & failureCallback, std::function<void(bool, std::optional<std::string>)> const & successCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showShareActionSheetWithOptions) static void showShareActionSheetWithOptions(ActionSheetManagerSpec_showShareActionSheetWithOptions_options && options, std::function<void(ActionSheetManagerSpec_showShareActionSheetWithOptions_failureCallback_error const &)> const & failureCallback, std::function<void(bool, std::optional<std::string>)> const & successCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "dismissActionSheet",
          "    REACT_METHOD(dismissActionSheet) void dismissActionSheet() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismissActionSheet) static void dismissActionSheet() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
