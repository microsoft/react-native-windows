
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

struct FileReaderModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(::React::JSValue, Promise<std::string>) noexcept>{0, L"readAsDataURL"},
      Method<void(::React::JSValue, std::string, Promise<std::string>) noexcept>{1, L"readAsText"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, FileReaderModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "readAsDataURL",
          "    REACT_METHOD(readAsDataURL) void readAsDataURL(::React::JSValue && data, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readAsDataURL) static void readAsDataURL(::React::JSValue && data, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "readAsText",
          "    REACT_METHOD(readAsText) void readAsText(::React::JSValue && data, std::string encoding, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readAsText) static void readAsText(::React::JSValue && data, std::string encoding, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
