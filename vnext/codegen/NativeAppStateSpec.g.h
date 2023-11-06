
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

struct AppStateSpec_AppStateConstants {
    std::string initialAppState;
};

struct AppStateSpec_AppState {
    std::string app_state;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AppStateSpec_AppStateConstants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"initialAppState", &AppStateSpec_AppStateConstants::initialAppState},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AppStateSpec_AppState*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"app_state", &AppStateSpec_AppState::app_state},
    };
    return fieldMap;
}

struct AppStateSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<AppStateSpec_AppState>, Callback<::React::JSValue>) noexcept>{0, L"getCurrentAppState"},
      Method<void(std::string) noexcept>{1, L"addListener"},
      Method<void(double) noexcept>{2, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AppStateSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getCurrentAppState",
          "    REACT_METHOD(getCurrentAppState) void getCurrentAppState(std::function<void(AppStateSpec_AppState const &)> const & success, std::function<void(::React::JSValue const &)> const & error) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentAppState) static void getCurrentAppState(std::function<void(AppStateSpec_AppState const &)> const & success, std::function<void(::React::JSValue const &)> const & error) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
