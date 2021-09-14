
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

struct NetworkingAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, std::string, double, React::JSValueArray, React::JSValue, std::string, bool, double, bool) noexcept>{0, L"sendRequest"},
      Method<void(double) noexcept>{1, L"abortRequest"},
      Method<void(Callback<bool>) noexcept>{2, L"clearCookies"},
      Method<void(std::string) noexcept>{3, L"addListener"},
      Method<void(double) noexcept>{4, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, NetworkingAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "sendRequest",
          "    REACT_METHOD(sendRequest) void sendRequest(std::string method, std::string url, double requestId, React::JSValueArray && headers, React::JSValue && data, std::string responseType, bool useIncrementalUpdates, double timeout, bool withCredentials) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(sendRequest) static void sendRequest(std::string method, std::string url, double requestId, React::JSValueArray && headers, React::JSValue && data, std::string responseType, bool useIncrementalUpdates, double timeout, bool withCredentials) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "abortRequest",
          "    REACT_METHOD(abortRequest) void abortRequest(double requestId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(abortRequest) static void abortRequest(double requestId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "clearCookies",
          "    REACT_METHOD(clearCookies) void clearCookies(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clearCookies) static void clearCookies(std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
