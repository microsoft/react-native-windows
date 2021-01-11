
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

struct ImageEditorSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, React::JSValueObject, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{0, L"cropImage"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImageEditorSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "cropImage",
          "    REACT_METHOD(cropImage) void cropImage(std::string uri, React::JSValueObject && cropData, std::function<void(React::JSValue const &)> const & successCallback, std::function<void(React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(cropImage) static void cropImage(std::string uri, React::JSValueObject && cropData, std::function<void(React::JSValue const &)> const & successCallback, std::function<void(React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
