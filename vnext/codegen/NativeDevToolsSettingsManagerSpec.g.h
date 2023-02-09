
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

struct DevToolsSettingsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"setConsolePatchSettings"},
      SyncMethod<std::optional<std::string>() noexcept>{1, L"getConsolePatchSettings"},
      Method<void(std::string) noexcept>{2, L"setProfilingSettings"},
      SyncMethod<std::optional<std::string>() noexcept>{3, L"getProfilingSettings"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DevToolsSettingsManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setConsolePatchSettings",
          "    REACT_METHOD(setConsolePatchSettings) void setConsolePatchSettings(std::string newConsolePatchSettings) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setConsolePatchSettings) static void setConsolePatchSettings(std::string newConsolePatchSettings) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getConsolePatchSettings",
          "    REACT_SYNC_METHOD(getConsolePatchSettings) std::optional<std::string> getConsolePatchSettings() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getConsolePatchSettings) static std::optional<std::string> getConsolePatchSettings() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "setProfilingSettings",
          "    REACT_METHOD(setProfilingSettings) void setProfilingSettings(std::string newProfilingSettings) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setProfilingSettings) static void setProfilingSettings(std::string newProfilingSettings) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getProfilingSettings",
          "    REACT_SYNC_METHOD(getProfilingSettings) std::optional<std::string> getProfilingSettings() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getProfilingSettings) static std::optional<std::string> getProfilingSettings() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
