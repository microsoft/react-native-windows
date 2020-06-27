
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

struct DevSettingsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"reload"},
      Method<void(std::string) noexcept>{1, L"reloadWithReason"},
      Method<void() noexcept>{2, L"onFastRefresh"},
      Method<void(bool) noexcept>{3, L"setHotLoadingEnabled"},
      Method<void(bool) noexcept>{4, L"setIsDebuggingRemotely"},
      Method<void(bool) noexcept>{5, L"setProfilingEnabled"},
      Method<void() noexcept>{6, L"toggleElementInspector"},
      Method<void(std::string) noexcept>{7, L"addMenuItem"},
      Method<void(std::string) noexcept>{8, L"addListener"},
      Method<void(double) noexcept>{9, L"removeListeners"},
      Method<void(bool) noexcept>{10, L"setIsShakeToShowDevMenuEnabled"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DevSettingsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "reload",
          "    REACT_METHOD(reload) void reload() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reload) static void reload() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "reloadWithReason",
          "    REACT_METHOD(reloadWithReason) void reloadWithReason(std::string reason) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reloadWithReason) static void reloadWithReason(std::string reason) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "onFastRefresh",
          "    REACT_METHOD(onFastRefresh) void onFastRefresh() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onFastRefresh) static void onFastRefresh() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setHotLoadingEnabled",
          "    REACT_METHOD(setHotLoadingEnabled) void setHotLoadingEnabled(bool isHotLoadingEnabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setHotLoadingEnabled) static void setHotLoadingEnabled(bool isHotLoadingEnabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "setIsDebuggingRemotely",
          "    REACT_METHOD(setIsDebuggingRemotely) void setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setIsDebuggingRemotely) static void setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "setProfilingEnabled",
          "    REACT_METHOD(setProfilingEnabled) void setProfilingEnabled(bool isProfilingEnabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setProfilingEnabled) static void setProfilingEnabled(bool isProfilingEnabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "toggleElementInspector",
          "    REACT_METHOD(toggleElementInspector) void toggleElementInspector() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(toggleElementInspector) static void toggleElementInspector() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "addMenuItem",
          "    REACT_METHOD(addMenuItem) void addMenuItem(std::string title) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addMenuItem) static void addMenuItem(std::string title) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "setIsShakeToShowDevMenuEnabled",
          "    REACT_METHOD(setIsShakeToShowDevMenuEnabled) void setIsShakeToShowDevMenuEnabled(bool enabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setIsShakeToShowDevMenuEnabled) static void setIsShakeToShowDevMenuEnabled(bool enabled) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
