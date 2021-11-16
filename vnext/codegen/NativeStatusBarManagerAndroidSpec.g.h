
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

REACT_STRUCT(StatusBarManagerAndroidSpec_Constants)
struct StatusBarManagerAndroidSpec_Constants {
    REACT_FIELD(HEIGHT)
    double HEIGHT;
    REACT_FIELD(DEFAULT_BACKGROUND_COLOR)
    double DEFAULT_BACKGROUND_COLOR;
};

struct StatusBarManagerAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<StatusBarManagerAndroidSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(double, bool) noexcept>{0, L"setColor"},
      Method<void(bool) noexcept>{1, L"setTranslucent"},
      Method<void(std::optional<std::string>) noexcept>{2, L"setStyle"},
      Method<void(bool) noexcept>{3, L"setHidden"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, StatusBarManagerAndroidSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, StatusBarManagerAndroidSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "StatusBarManagerAndroidSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) StatusBarManagerAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static StatusBarManagerAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setColor",
          "    REACT_METHOD(setColor) void setColor(double color, bool animated) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setColor) static void setColor(double color, bool animated) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setTranslucent",
          "    REACT_METHOD(setTranslucent) void setTranslucent(bool translucent) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setTranslucent) static void setTranslucent(bool translucent) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "setStyle",
          "    REACT_METHOD(setStyle) void setStyle(std::optional<std::string> statusBarStyle) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setStyle) static void setStyle(std::optional<std::string> statusBarStyle) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setHidden",
          "    REACT_METHOD(setHidden) void setHidden(bool hidden) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setHidden) static void setHidden(bool hidden) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
