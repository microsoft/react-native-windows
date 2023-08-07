
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers {
    std::optional<double> extraSmall;
    std::optional<double> small;
    std::optional<double> medium;
    std::optional<double> large;
    std::optional<double> extraLarge;
    std::optional<double> extraExtraLarge;
    std::optional<double> extraExtraExtraLarge;
    std::optional<double> accessibilityMedium;
    std::optional<double> accessibilityLarge;
    std::optional<double> accessibilityExtraLarge;
    std::optional<double> accessibilityExtraExtraLarge;
    std::optional<double> accessibilityExtraExtraExtraLarge;
};

struct AccessibilityManagerSpec_announceForAccessibilityWithOptions_options {
    std::optional<bool> queue;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"extraSmall", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::extraSmall},
        {L"small", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::small},
        {L"medium", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::medium},
        {L"large", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::large},
        {L"extraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::extraLarge},
        {L"extraExtraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::extraExtraLarge},
        {L"extraExtraExtraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::extraExtraExtraLarge},
        {L"accessibilityMedium", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::accessibilityMedium},
        {L"accessibilityLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::accessibilityLarge},
        {L"accessibilityExtraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::accessibilityExtraLarge},
        {L"accessibilityExtraExtraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::accessibilityExtraExtraLarge},
        {L"accessibilityExtraExtraExtraLarge", &AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers::accessibilityExtraExtraExtraLarge},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AccessibilityManagerSpec_announceForAccessibilityWithOptions_options*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"queue", &AccessibilityManagerSpec_announceForAccessibilityWithOptions_options::queue},
    };
    return fieldMap;
}

struct AccessibilityManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{0, L"getCurrentBoldTextState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{1, L"getCurrentGrayscaleState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{2, L"getCurrentInvertColorsState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{3, L"getCurrentReduceMotionState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{4, L"getCurrentPrefersCrossFadeTransitionsState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{5, L"getCurrentReduceTransparencyState"},
      Method<void(Callback<bool>, Callback<::React::JSValue>) noexcept>{6, L"getCurrentVoiceOverState"},
      Method<void(AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers) noexcept>{7, L"setAccessibilityContentSizeMultipliers"},
      Method<void(double) noexcept>{8, L"setAccessibilityFocus"},
      Method<void(std::string) noexcept>{9, L"announceForAccessibility"},
      Method<void(std::string, AccessibilityManagerSpec_announceForAccessibilityWithOptions_options) noexcept>{10, L"announceForAccessibilityWithOptions"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AccessibilityManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getCurrentBoldTextState",
          "    REACT_METHOD(getCurrentBoldTextState) void getCurrentBoldTextState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentBoldTextState) static void getCurrentBoldTextState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getCurrentGrayscaleState",
          "    REACT_METHOD(getCurrentGrayscaleState) void getCurrentGrayscaleState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentGrayscaleState) static void getCurrentGrayscaleState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getCurrentInvertColorsState",
          "    REACT_METHOD(getCurrentInvertColorsState) void getCurrentInvertColorsState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentInvertColorsState) static void getCurrentInvertColorsState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getCurrentReduceMotionState",
          "    REACT_METHOD(getCurrentReduceMotionState) void getCurrentReduceMotionState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentReduceMotionState) static void getCurrentReduceMotionState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getCurrentPrefersCrossFadeTransitionsState",
          "    REACT_METHOD(getCurrentPrefersCrossFadeTransitionsState) void getCurrentPrefersCrossFadeTransitionsState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentPrefersCrossFadeTransitionsState) static void getCurrentPrefersCrossFadeTransitionsState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getCurrentReduceTransparencyState",
          "    REACT_METHOD(getCurrentReduceTransparencyState) void getCurrentReduceTransparencyState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentReduceTransparencyState) static void getCurrentReduceTransparencyState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getCurrentVoiceOverState",
          "    REACT_METHOD(getCurrentVoiceOverState) void getCurrentVoiceOverState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getCurrentVoiceOverState) static void getCurrentVoiceOverState(std::function<void(bool)> const & onSuccess, std::function<void(::React::JSValue const &)> const & onError) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "setAccessibilityContentSizeMultipliers",
          "    REACT_METHOD(setAccessibilityContentSizeMultipliers) void setAccessibilityContentSizeMultipliers(AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers && JSMultipliers) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setAccessibilityContentSizeMultipliers) static void setAccessibilityContentSizeMultipliers(AccessibilityManagerSpec_setAccessibilityContentSizeMultipliers_JSMultipliers && JSMultipliers) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "setAccessibilityFocus",
          "    REACT_METHOD(setAccessibilityFocus) void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setAccessibilityFocus) static void setAccessibilityFocus(double reactTag) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "announceForAccessibility",
          "    REACT_METHOD(announceForAccessibility) void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(announceForAccessibility) static void announceForAccessibility(std::string announcement) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "announceForAccessibilityWithOptions",
          "    REACT_METHOD(announceForAccessibilityWithOptions) void announceForAccessibilityWithOptions(std::string announcement, AccessibilityManagerSpec_announceForAccessibilityWithOptions_options && options) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(announceForAccessibilityWithOptions) static void announceForAccessibilityWithOptions(std::string announcement, AccessibilityManagerSpec_announceForAccessibilityWithOptions_options && options) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
