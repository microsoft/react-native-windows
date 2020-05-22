
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

struct AsyncStorageSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(React::JSValueArray, Callback<React::JSValue>) noexcept>{0, L"multiGet"},
      Method<void(React::JSValueArray, Callback<React::JSValue>) noexcept>{1, L"multiSet"},
      Method<void(React::JSValueArray, Callback<React::JSValue>) noexcept>{2, L"multiMerge"},
      Method<void(React::JSValueArray, Callback<React::JSValue>) noexcept>{3, L"multiRemove"},
      Method<void(Callback<React::JSValue>) noexcept>{4, L"clear"},
      Method<void(Callback<React::JSValue>) noexcept>{5, L"getAllKeys"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AsyncStorageSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "multiGet",
          "    REACT_METHOD(multiGet) void multiGet(React::JSValueArray && keys, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiGet) static void multiGet(React::JSValueArray && keys, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "multiSet",
          "    REACT_METHOD(multiSet) void multiSet(React::JSValueArray && kvPairs, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiSet) static void multiSet(React::JSValueArray && kvPairs, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "multiMerge",
          "    REACT_METHOD(multiMerge) void multiMerge(React::JSValueArray && kvPairs, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiMerge) static void multiMerge(React::JSValueArray && kvPairs, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "multiRemove",
          "    REACT_METHOD(multiRemove) void multiRemove(React::JSValueArray && keys, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiRemove) static void multiRemove(React::JSValueArray && keys, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "clear",
          "    REACT_METHOD(clear) void clear(std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clear) static void clear(std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getAllKeys",
          "    REACT_METHOD(getAllKeys) void getAllKeys(std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getAllKeys) static void getAllKeys(std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
