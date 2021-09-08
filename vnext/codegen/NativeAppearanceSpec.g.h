
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

struct AppearanceSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<std::optional<std::string>() noexcept>{0, L"getColorScheme"},
      Method<void(std::string) noexcept>{1, L"addListener"},
      Method<void(double) noexcept>{2, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AppearanceSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getColorScheme",
          "    REACT_SYNC_METHOD(getColorScheme) std::optional<std::string> getColorScheme() noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getColorScheme) static std::optional<std::string> getColorScheme() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
