
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

REACT_STRUCT(AppThemeSpec_HighContrastColors)
struct AppThemeSpec_HighContrastColors {
    REACT_FIELD(ButtonFaceColor)
    std::string ButtonFaceColor;
    REACT_FIELD(ButtonTextColor)
    std::string ButtonTextColor;
    REACT_FIELD(GrayTextColor)
    std::string GrayTextColor;
    REACT_FIELD(HighlightColor)
    std::string HighlightColor;
    REACT_FIELD(HighlightTextColor)
    std::string HighlightTextColor;
    REACT_FIELD(HotlightColor)
    std::string HotlightColor;
    REACT_FIELD(WindowColor)
    std::string WindowColor;
    REACT_FIELD(WindowTextColor)
    std::string WindowTextColor;
};

REACT_STRUCT(AppThemeSpec_AppThemeData)
struct AppThemeSpec_AppThemeData {
    REACT_FIELD(isHighContrast)
    bool isHighContrast;
    REACT_FIELD(highContrastColors)
    AppThemeSpec_HighContrastColors highContrastColors;
};

struct AppThemeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AppThemeSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
