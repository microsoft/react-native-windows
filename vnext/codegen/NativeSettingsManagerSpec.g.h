
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

REACT_STRUCT(SettingsManagerSpec_Constants)
struct SettingsManagerSpec_Constants {
    REACT_FIELD(settings)
    ::React::JSValue settings;
};

struct SettingsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<SettingsManagerSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(::React::JSValue) noexcept>{0, L"setValues"},
      Method<void(std::vector<std::string>) noexcept>{1, L"deleteValues"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, SettingsManagerSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, SettingsManagerSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "SettingsManagerSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) SettingsManagerSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static SettingsManagerSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setValues",
          "    REACT_METHOD(setValues) void setValues(::React::JSValue && values) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setValues) static void setValues(::React::JSValue && values) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "deleteValues",
          "    REACT_METHOD(deleteValues) void deleteValues(std::vector<std::string> const & values) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(deleteValues) static void deleteValues(std::vector<std::string> const & values) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
