
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

struct DevMenuSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"show"},
      Method<void() noexcept>{1, L"reload"},
      Method<void(bool) noexcept>{2, L"debugRemotely"},
      Method<void(bool) noexcept>{3, L"setProfilingEnabled"},
      Method<void(bool) noexcept>{4, L"setHotLoadingEnabled"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DevMenuSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "show",
          "    REACT_METHOD(show) void show() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(show) static void show() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "reload",
          "    REACT_METHOD(reload) void reload() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reload) static void reload() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "debugRemotely",
          "    REACT_METHOD(debugRemotely) void debugRemotely(bool enableDebug) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(debugRemotely) static void debugRemotely(bool enableDebug) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setProfilingEnabled",
          "    REACT_METHOD(setProfilingEnabled) void setProfilingEnabled(bool enabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setProfilingEnabled) static void setProfilingEnabled(bool enabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "setHotLoadingEnabled",
          "    REACT_METHOD(setHotLoadingEnabled) void setHotLoadingEnabled(bool enabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setHotLoadingEnabled) static void setHotLoadingEnabled(bool enabled) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
