
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

REACT_STRUCT(DeviceInfoSpec_DisplayMetrics)
struct DeviceInfoSpec_DisplayMetrics {
    REACT_FIELD(width)
    double width;
    REACT_FIELD(height)
    double height;
    REACT_FIELD(scale)
    double scale;
    REACT_FIELD(fontScale)
    double fontScale;
};

REACT_STRUCT(DeviceInfoSpec_DisplayMetricsAndroid)
struct DeviceInfoSpec_DisplayMetricsAndroid {
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

REACT_STRUCT(DeviceInfoSpec_DimensionsPayload)
struct DeviceInfoSpec_DimensionsPayload {
    REACT_FIELD(window)
    std::optional<DeviceInfoSpec_DisplayMetrics> window;
    REACT_FIELD(screen)
    std::optional<DeviceInfoSpec_DisplayMetrics> screen;
    REACT_FIELD(windowPhysicalPixels)
    std::optional<DeviceInfoSpec_DisplayMetricsAndroid> windowPhysicalPixels;
    REACT_FIELD(screenPhysicalPixels)
    std::optional<DeviceInfoSpec_DisplayMetricsAndroid> screenPhysicalPixels;
};

struct DeviceInfoSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DeviceInfoSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
