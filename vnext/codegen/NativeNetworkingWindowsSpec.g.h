
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

REACT_STRUCT(NetworkingWindowsSpec_sendRequest_query)
struct NetworkingWindowsSpec_sendRequest_query {
    REACT_FIELD(method)
    std::string method;
    REACT_FIELD(url)
    std::string url;
    REACT_FIELD(requestId)
    double requestId;
    REACT_FIELD(data)
    ::React::JSValue data;
    REACT_FIELD(headers)
    ::React::JSValue headers;
    REACT_FIELD(responseType)
    std::string responseType;
    REACT_FIELD(incrementalUpdates)
    bool incrementalUpdates;
    REACT_FIELD(timeout)
    double timeout;
    REACT_FIELD(withCredentials)
    bool withCredentials;
};

struct NetworkingWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(NetworkingWindowsSpec_sendRequest_query, Callback<double>) noexcept>{0, L"sendRequest"},
      Method<void(double) noexcept>{1, L"abortRequest"},
      Method<void(Callback<bool>) noexcept>{2, L"clearCookies"},
      Method<void(std::string) noexcept>{3, L"addListener"},
      Method<void(double) noexcept>{4, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, NetworkingWindowsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "sendRequest",
          "    REACT_METHOD(sendRequest) void sendRequest(NetworkingWindowsSpec_sendRequest_query && query, std::function<void(double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(sendRequest) static void sendRequest(NetworkingWindowsSpec_sendRequest_query && query, std::function<void(double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "abortRequest",
          "    REACT_METHOD(abortRequest) void abortRequest(double requestId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(abortRequest) static void abortRequest(double requestId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "clearCookies",
          "    REACT_METHOD(clearCookies) void clearCookies(std::function<void(bool)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearCookies) static void clearCookies(std::function<void(bool)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
