
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

struct AppThemeSpec_HighContrastColors {
    std::string ButtonFaceColor;
    std::string ButtonTextColor;
    std::string GrayTextColor;
    std::string HighlightColor;
    std::string HighlightTextColor;
    std::string HotlightColor;
    std::string WindowColor;
    std::string WindowTextColor;
};

struct AppThemeSpec_AppThemeData {
    bool isHighContrast;
    AppThemeSpec_HighContrastColors highContrastColors;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AppThemeSpec_HighContrastColors*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"ButtonFaceColor", &AppThemeSpec_HighContrastColors::ButtonFaceColor},
        {L"ButtonTextColor", &AppThemeSpec_HighContrastColors::ButtonTextColor},
        {L"GrayTextColor", &AppThemeSpec_HighContrastColors::GrayTextColor},
        {L"HighlightColor", &AppThemeSpec_HighContrastColors::HighlightColor},
        {L"HighlightTextColor", &AppThemeSpec_HighContrastColors::HighlightTextColor},
        {L"HotlightColor", &AppThemeSpec_HighContrastColors::HotlightColor},
        {L"WindowColor", &AppThemeSpec_HighContrastColors::WindowColor},
        {L"WindowTextColor", &AppThemeSpec_HighContrastColors::WindowTextColor},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(AppThemeSpec_AppThemeData*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isHighContrast", &AppThemeSpec_AppThemeData::isHighContrast},
        {L"highContrastColors", &AppThemeSpec_AppThemeData::highContrastColors},
    };
    return fieldMap;
}

struct AppThemeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AppThemeSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
