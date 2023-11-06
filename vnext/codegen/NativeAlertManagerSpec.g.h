
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

struct AlertManagerSpec_Args {
    std::optional<std::string> title;
    std::optional<std::string> message;
    std::optional<std::vector<::React::JSValue>> buttons;
    std::optional<std::string> type;
    std::optional<std::string> defaultValue;
    std::optional<std::string> cancelButtonKey;
    std::optional<std::string> destructiveButtonKey;
    std::optional<std::string> preferredButtonKey;
    std::optional<std::string> keyboardType;
    std::optional<std::string> userInterfaceStyle;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AlertManagerSpec_Args*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"title", &AlertManagerSpec_Args::title},
        {L"message", &AlertManagerSpec_Args::message},
        {L"buttons", &AlertManagerSpec_Args::buttons},
        {L"type", &AlertManagerSpec_Args::type},
        {L"defaultValue", &AlertManagerSpec_Args::defaultValue},
        {L"cancelButtonKey", &AlertManagerSpec_Args::cancelButtonKey},
        {L"destructiveButtonKey", &AlertManagerSpec_Args::destructiveButtonKey},
        {L"preferredButtonKey", &AlertManagerSpec_Args::preferredButtonKey},
        {L"keyboardType", &AlertManagerSpec_Args::keyboardType},
        {L"userInterfaceStyle", &AlertManagerSpec_Args::userInterfaceStyle},
    };
    return fieldMap;
}

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
          "    REACT_METHOD(alertWithArgs) void alertWithArgs(AlertManagerSpec_Args && args, std::function<void(double, std::string)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(alertWithArgs) static void alertWithArgs(AlertManagerSpec_Args && args, std::function<void(double, std::string)> const & callback) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
