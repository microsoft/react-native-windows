
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
  struct DisplayMetrics {
      double width;
      double height;
      double scale;
      double fontScale;
  };

  struct DisplayMetricsAndroid {
      double width;
      double height;
      double scale;
      double fontScale;
      double densityDpi;
  };

  struct DimensionsPayload {
      std::optional<DisplayMetrics> window;
      std::optional<DisplayMetrics> screen;
      std::optional<DisplayMetricsAndroid> windowPhysicalPixels;
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
