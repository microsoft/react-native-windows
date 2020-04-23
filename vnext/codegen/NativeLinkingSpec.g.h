
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

struct LinkingSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Promise<React::JSValue>) noexcept>{0, L"getInitialURL"},
      Method<void(std::string, Promise<React::JSValue>) noexcept>{1, L"canOpenURL"},
      Method<void(std::string, Promise<React::JSValue>) noexcept>{2, L"openURL"},
      Method<void(Promise<React::JSValue>) noexcept>{3, L"openSettings"},
      Method<void(std::string, JSValueArray, Promise<React::JSValue>) noexcept>{4, L"sendIntent"},
      Method<void(std::string) noexcept>{5, L"addListener"},
      Method<void(double) noexcept>{6, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, LinkingSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getInitialURL",
          "    REACT_METHOD(getInitialURL) void getInitialURL(React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getInitialURL) static void getInitialURL(React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "canOpenURL",
          "    REACT_METHOD(canOpenURL) void canOpenURL(std::string url, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(canOpenURL) static void canOpenURL(std::string url, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "openURL",
          "    REACT_METHOD(openURL) void openURL(std::string url, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(openURL) static void openURL(std::string url, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "openSettings",
          "    REACT_METHOD(openSettings) void openSettings(React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(openSettings) static void openSettings(React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "sendIntent",
          "    REACT_METHOD(sendIntent) void sendIntent(std::string action, React::JSValueArray && extras, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(sendIntent) static void sendIntent(std::string action, React::JSValueArray && extras, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
