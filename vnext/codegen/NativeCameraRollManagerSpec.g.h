
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

struct CameraRollManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(React::JSValueObject, Promise<React::JSValue>) noexcept>{0, L"getPhotos"},
      Method<void(std::string, std::string, Promise<React::JSValue>) noexcept>{1, L"saveToCameraRoll"},
      Method<void(React::JSValueArray, Promise<React::JSValue>) noexcept>{2, L"deletePhotos"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, CameraRollManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getPhotos",
          "    REACT_METHOD(getPhotos) void getPhotos(React::JSValueObject && params, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getPhotos) static void getPhotos(React::JSValueObject && params, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "saveToCameraRoll",
          "    REACT_METHOD(saveToCameraRoll) void saveToCameraRoll(std::string uri, std::string type, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(saveToCameraRoll) static void saveToCameraRoll(std::string uri, std::string type, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "deletePhotos",
          "    REACT_METHOD(deletePhotos) void deletePhotos(React::JSValueArray && assets, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(deletePhotos) static void deletePhotos(React::JSValueArray && assets, React::ReactPromise<React::JSValue> &&result) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
