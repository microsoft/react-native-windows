
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

struct ImageStoreAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, Callback<std::string>, Callback<std::string>) noexcept>{0, L"getBase64ForTag"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImageStoreAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getBase64ForTag",
          "    REACT_METHOD(getBase64ForTag) void getBase64ForTag(std::string uri, std::function<void(std::string)> const & successCallback, std::function<void(std::string)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getBase64ForTag) static void getBase64ForTag(std::string uri, std::function<void(std::string)> const & successCallback, std::function<void(std::string)> const & errorCallback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
