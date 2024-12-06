
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {


struct ReactDevToolsSettingsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"setGlobalHookSettings"},
      SyncMethod<std::optional<std::string>() noexcept>{1, L"getGlobalHookSettings"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ReactDevToolsSettingsManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setGlobalHookSettings",
          "    REACT_METHOD(setGlobalHookSettings) void setGlobalHookSettings(std::string settings) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setGlobalHookSettings) static void setGlobalHookSettings(std::string settings) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getGlobalHookSettings",
          "    REACT_SYNC_METHOD(getGlobalHookSettings) std::optional<std::string> getGlobalHookSettings() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getGlobalHookSettings) static std::optional<std::string> getGlobalHookSettings() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
