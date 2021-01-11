
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

struct AccessibilityInfoSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<React::JSValue>) noexcept>{0, L"isReduceMotionEnabled"},
      Method<void(Callback<React::JSValue>) noexcept>{1, L"isTouchExplorationEnabled"},
      Method<void(double) noexcept>{2, L"setAccessibilityFocus"},
      Method<void(std::string) noexcept>{3, L"announceForAccessibility"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AccessibilityInfoSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "isReduceMotionEnabled",
          "    REACT_METHOD(isReduceMotionEnabled) void isReduceMotionEnabled(std::function<void(React::JSValue const &)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(isReduceMotionEnabled) static void isReduceMotionEnabled(std::function<void(React::JSValue const &)> const & onSuccess) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "isTouchExplorationEnabled",
          "    REACT_METHOD(isTouchExplorationEnabled) void isTouchExplorationEnabled(std::function<void(React::JSValue const &)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(isTouchExplorationEnabled) static void isTouchExplorationEnabled(std::function<void(React::JSValue const &)> const & onSuccess) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "setAccessibilityFocus",
          "    REACT_METHOD(setAccessibilityFocus) void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAccessibilityFocus) static void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "announceForAccessibility",
          "    REACT_METHOD(announceForAccessibility) void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(announceForAccessibility) static void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
