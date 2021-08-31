
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

struct DeviceInfoSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  REACT_STRUCT(DisplayMetrics)
  struct DisplayMetrics {
      REACT_FIELD(width)
      double width;
      REACT_FIELD(height)
      double height;
      REACT_FIELD(scale)
      double scale;
      REACT_FIELD(fontScale)
      double fontScale;
  };

  REACT_STRUCT(DisplayMetricsAndroid)
  struct DisplayMetricsAndroid {
      REACT_FIELD(width)
      double width;
      REACT_FIELD(height)
      double height;
      REACT_FIELD(scale)
      double scale;
      REACT_FIELD(fontScale)
      double fontScale;
      REACT_FIELD(densityDpi)
      double densityDpi;
  };

  REACT_STRUCT(DimensionsPayload)
  struct DimensionsPayload {
      REACT_FIELD(window)
      std::optional<DisplayMetrics> window;
      REACT_FIELD(screen)
      std::optional<DisplayMetrics> screen;
      REACT_FIELD(windowPhysicalPixels)
      std::optional<DisplayMetricsAndroid> windowPhysicalPixels;
      REACT_FIELD(screenPhysicalPixels)
      std::optional<DisplayMetricsAndroid> screenPhysicalPixels;
  };

  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DeviceInfoSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
