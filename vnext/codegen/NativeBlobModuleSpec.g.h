
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

struct BlobModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"addNetworkingHandler"},
      Method<void(double) noexcept>{1, L"addWebSocketHandler"},
      Method<void(double) noexcept>{2, L"removeWebSocketHandler"},
      Method<void(JSValueObject, double) noexcept>{3, L"sendOverSocket"},
      Method<void(JSValueArray, std::string) noexcept>{4, L"createFromParts"},
      Method<void(std::string) noexcept>{5, L"release"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, BlobModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "addNetworkingHandler",
          "    REACT_METHOD(addNetworkingHandler) void addNetworkingHandler() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addNetworkingHandler) static void addNetworkingHandler() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "addWebSocketHandler",
          "    REACT_METHOD(addWebSocketHandler) void addWebSocketHandler(double id) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addWebSocketHandler) static void addWebSocketHandler(double id) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "removeWebSocketHandler",
          "    REACT_METHOD(removeWebSocketHandler) void removeWebSocketHandler(double id) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeWebSocketHandler) static void removeWebSocketHandler(double id) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "sendOverSocket",
          "    REACT_METHOD(sendOverSocket) void sendOverSocket(React::JSValueObject && blob, double socketID) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(sendOverSocket) static void sendOverSocket(React::JSValueObject && blob, double socketID) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "createFromParts",
          "    REACT_METHOD(createFromParts) void createFromParts(React::JSValueArray && parts, std::string withId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createFromParts) static void createFromParts(React::JSValueArray && parts, std::string withId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "release",
          "    REACT_METHOD(release) void release(std::string blobId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(release) static void release(std::string blobId) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
