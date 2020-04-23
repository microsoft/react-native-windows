
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

struct AccessibilityManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{0, L"getCurrentBoldTextState"},
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{1, L"getCurrentGrayscaleState"},
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{2, L"getCurrentInvertColorsState"},
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{3, L"getCurrentReduceMotionState"},
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{4, L"getCurrentReduceTransparencyState"},
      Method<void(Callback<JSValue>,Callback<JSValue>) noexcept>{5, L"getCurrentVoiceOverState"},
      Method<void(JSValueObject) noexcept>{6, L"setAccessibilityContentSizeMultipliers"},
      Method<void(double) noexcept>{7, L"setAccessibilityFocus"},
      Method<void(std::string) noexcept>{8, L"announceForAccessibility"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AccessibilityManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getCurrentBoldTextState",
          "    REACT_METHOD(getCurrentBoldTextState) void getCurrentBoldTextState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getCurrentGrayscaleState",
          "    REACT_METHOD(getCurrentGrayscaleState) void getCurrentGrayscaleState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getCurrentInvertColorsState",
          "    REACT_METHOD(getCurrentInvertColorsState) void getCurrentInvertColorsState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getCurrentReduceMotionState",
          "    REACT_METHOD(getCurrentReduceMotionState) void getCurrentReduceMotionState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getCurrentReduceTransparencyState",
          "    REACT_METHOD(getCurrentReduceTransparencyState) void getCurrentReduceTransparencyState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getCurrentVoiceOverState",
          "    REACT_METHOD(getCurrentVoiceOverState) void getCurrentVoiceOverState(std::function<void(React::JSValue const &)> const & onSuccess, std::function<void(React::JSValue const &)> const & onError) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "setAccessibilityContentSizeMultipliers",
          "    REACT_METHOD(setAccessibilityContentSizeMultipliers) void setAccessibilityContentSizeMultipliers(React::JSValueObject && JSMultipliers) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "setAccessibilityFocus",
          "    REACT_METHOD(setAccessibilityFocus) void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }}");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "announceForAccessibility",
          "    REACT_METHOD(announceForAccessibility) void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }}");
  }
};

} // namespace Microsoft::ReactNativeSpecs
