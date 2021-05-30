
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

struct LogBoxSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"show"},
      Method<void() noexcept>{1, L"hide"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, LogBoxSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "show",
          "    REACT_METHOD(show) void show() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(show) static void show() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "hide",
          "    REACT_METHOD(hide) void hide() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(hide) static void hide() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
