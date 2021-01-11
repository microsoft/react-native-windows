
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

struct ImageLoaderAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double) noexcept>{0, L"abortRequest"},
      Method<void(std::string, Promise<React::JSValue>) noexcept>{1, L"getSize"},
      Method<void(std::string, React::JSValueObject, Promise<React::JSValue>) noexcept>{2, L"getSizeWithHeaders"},
      Method<void(std::string, double, Promise<React::JSValue>) noexcept>{3, L"prefetchImage"},
      Method<void(React::JSValueArray, Promise<React::JSValue>) noexcept>{4, L"queryCache"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImageLoaderAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "abortRequest",
          "    REACT_METHOD(abortRequest) void abortRequest(double requestId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(abortRequest) static void abortRequest(double requestId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getSize",
          "    REACT_METHOD(getSize) void getSize(std::string uri, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getSize) static void getSize(std::string uri, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getSizeWithHeaders",
          "    REACT_METHOD(getSizeWithHeaders) void getSizeWithHeaders(std::string uri, React::JSValueObject && headers, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getSizeWithHeaders) static void getSizeWithHeaders(std::string uri, React::JSValueObject && headers, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "prefetchImage",
          "    REACT_METHOD(prefetchImage) void prefetchImage(std::string uri, double requestId, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(prefetchImage) static void prefetchImage(std::string uri, double requestId, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "queryCache",
          "    REACT_METHOD(queryCache) void queryCache(React::JSValueArray && uris, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(queryCache) static void queryCache(React::JSValueArray && uris, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
