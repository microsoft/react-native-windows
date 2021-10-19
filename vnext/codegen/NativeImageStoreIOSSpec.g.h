
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

REACT_STRUCT(ImageStoreIOSSpec_getBase64ForTag_errorCallback_error)
struct ImageStoreIOSSpec_getBase64ForTag_errorCallback_error {
    REACT_FIELD(message)
    std::string message;
};

REACT_STRUCT(ImageStoreIOSSpec_addImageFromBase64_errorCallback_error)
struct ImageStoreIOSSpec_addImageFromBase64_errorCallback_error {
    REACT_FIELD(message)
    std::string message;
};

struct ImageStoreIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, Callback<std::string>, Callback<ImageStoreIOSSpec_getBase64ForTag_errorCallback_error>) noexcept>{0, L"getBase64ForTag"},
      Method<void(std::string, Callback<bool>) noexcept>{1, L"hasImageForTag"},
      Method<void(std::string) noexcept>{2, L"removeImageForTag"},
      Method<void(std::string, Callback<std::string>, Callback<ImageStoreIOSSpec_addImageFromBase64_errorCallback_error>) noexcept>{3, L"addImageFromBase64"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ImageStoreIOSSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getBase64ForTag",
          "    REACT_METHOD(getBase64ForTag) void getBase64ForTag(std::string uri, std::function<void(std::string)> const & successCallback, std::function<void(ImageStoreIOSSpec_getBase64ForTag_errorCallback_error const &)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getBase64ForTag) static void getBase64ForTag(std::string uri, std::function<void(std::string)> const & successCallback, std::function<void(ImageStoreIOSSpec_getBase64ForTag_errorCallback_error const &)> const & errorCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "hasImageForTag",
          "    REACT_METHOD(hasImageForTag) void hasImageForTag(std::string uri, std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(hasImageForTag) static void hasImageForTag(std::string uri, std::function<void(bool)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "removeImageForTag",
          "    REACT_METHOD(removeImageForTag) void removeImageForTag(std::string uri) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeImageForTag) static void removeImageForTag(std::string uri) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "addImageFromBase64",
          "    REACT_METHOD(addImageFromBase64) void addImageFromBase64(std::string base64ImageData, std::function<void(std::string)> const & successCallback, std::function<void(ImageStoreIOSSpec_addImageFromBase64_errorCallback_error const &)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addImageFromBase64) static void addImageFromBase64(std::string base64ImageData, std::function<void(std::string)> const & successCallback, std::function<void(ImageStoreIOSSpec_addImageFromBase64_errorCallback_error const &)> const & errorCallback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
