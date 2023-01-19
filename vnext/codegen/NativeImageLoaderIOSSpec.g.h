
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

REACT_STRUCT(ImageLoaderIOSSpec_getSizeWithHeaders_returnType)
struct ImageLoaderIOSSpec_getSizeWithHeaders_returnType {
    REACT_FIELD(width)
    double width;
    REACT_FIELD(height)
    double height;
};

struct ImageLoaderIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, Promise<std::vector<double>>) noexcept>{0, L"getSize"},
      Method<void(std::string, ::React::JSValue, Promise<ImageLoaderIOSSpec_getSizeWithHeaders_returnType>) noexcept>{1, L"getSizeWithHeaders"},
      Method<void(std::string, Promise<bool>) noexcept>{2, L"prefetchImage"},
      Method<void(std::string, std::string, double, Promise<bool>) noexcept>{3, L"prefetchImageWithMetadata"},
      Method<void(std::vector<std::string>, Promise<::React::JSValue>) noexcept>{4, L"queryCache"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImageLoaderIOSSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getSize",
          "    REACT_METHOD(getSize) void getSize(std::string uri, ::React::ReactPromise<std::vector<double>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getSize) static void getSize(std::string uri, ::React::ReactPromise<std::vector<double>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getSizeWithHeaders",
          "    REACT_METHOD(getSizeWithHeaders) void getSizeWithHeaders(std::string uri, ::React::JSValue && headers, ::React::ReactPromise<ImageLoaderIOSSpec_getSizeWithHeaders_returnType> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getSizeWithHeaders) static void getSizeWithHeaders(std::string uri, ::React::JSValue && headers, ::React::ReactPromise<ImageLoaderIOSSpec_getSizeWithHeaders_returnType> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "prefetchImage",
          "    REACT_METHOD(prefetchImage) void prefetchImage(std::string uri, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(prefetchImage) static void prefetchImage(std::string uri, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "prefetchImageWithMetadata",
          "    REACT_METHOD(prefetchImageWithMetadata) void prefetchImageWithMetadata(std::string uri, std::string queryRootName, double rootTag, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(prefetchImageWithMetadata) static void prefetchImageWithMetadata(std::string uri, std::string queryRootName, double rootTag, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "queryCache",
          "    REACT_METHOD(queryCache) void queryCache(std::vector<std::string> const & uris, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(queryCache) static void queryCache(std::vector<std::string> const & uris, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
