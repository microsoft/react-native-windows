
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
      Method<void(Callback<bool>) noexcept>{0, L"isReduceMotionEnabled"},
      Method<void(Callback<bool>) noexcept>{1, L"isTouchExplorationEnabled"},
      Method<void(Callback<bool>) noexcept>{2, L"isAccessibilityServiceEnabled"},
      Method<void(double) noexcept>{3, L"setAccessibilityFocus"},
      Method<void(std::string) noexcept>{4, L"announceForAccessibility"},
      Method<void(double, Callback<double>) noexcept>{5, L"getRecommendedTimeoutMillis"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AccessibilityInfoSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "isReduceMotionEnabled",
          "    REACT_METHOD(isReduceMotionEnabled) void isReduceMotionEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(isReduceMotionEnabled) static void isReduceMotionEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "isTouchExplorationEnabled",
          "    REACT_METHOD(isTouchExplorationEnabled) void isTouchExplorationEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(isTouchExplorationEnabled) static void isTouchExplorationEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "isAccessibilityServiceEnabled",
          "    REACT_METHOD(isAccessibilityServiceEnabled) void isAccessibilityServiceEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(isAccessibilityServiceEnabled) static void isAccessibilityServiceEnabled(std::function<void(bool)> const & onSuccess) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setAccessibilityFocus",
          "    REACT_METHOD(setAccessibilityFocus) void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAccessibilityFocus) static void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "announceForAccessibility",
          "    REACT_METHOD(announceForAccessibility) void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(announceForAccessibility) static void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getRecommendedTimeoutMillis",
          "    REACT_METHOD(getRecommendedTimeoutMillis) void getRecommendedTimeoutMillis(double mSec, std::function<void(double)> const & onSuccess) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getRecommendedTimeoutMillis) static void getRecommendedTimeoutMillis(double mSec, std::function<void(double)> const & onSuccess) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
