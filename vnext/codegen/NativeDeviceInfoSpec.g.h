
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

struct DeviceInfoSpec_DisplayMetrics {
    double width;
    double height;
    double scale;
    double fontScale;
};

struct DeviceInfoSpec_DisplayMetricsAndroid {
    double width;
    double height;
    double scale;
    double fontScale;
    double densityDpi;
};

struct DeviceInfoSpec_DimensionsPayload {
    std::optional<DeviceInfoSpec_DisplayMetrics> window;
    std::optional<DeviceInfoSpec_DisplayMetrics> screen;
    std::optional<DeviceInfoSpec_DisplayMetricsAndroid> windowPhysicalPixels;
    std::optional<DeviceInfoSpec_DisplayMetricsAndroid> screenPhysicalPixels;
};

struct DeviceInfoSpec_DeviceInfoConstants {
    DeviceInfoSpec_DimensionsPayload Dimensions;
    std::optional<bool> isIPhoneX_deprecated;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DeviceInfoSpec_DisplayMetrics*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"width", &DeviceInfoSpec_DisplayMetrics::width},
        {L"height", &DeviceInfoSpec_DisplayMetrics::height},
        {L"scale", &DeviceInfoSpec_DisplayMetrics::scale},
        {L"fontScale", &DeviceInfoSpec_DisplayMetrics::fontScale},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DeviceInfoSpec_DisplayMetricsAndroid*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"width", &DeviceInfoSpec_DisplayMetricsAndroid::width},
        {L"height", &DeviceInfoSpec_DisplayMetricsAndroid::height},
        {L"scale", &DeviceInfoSpec_DisplayMetricsAndroid::scale},
        {L"fontScale", &DeviceInfoSpec_DisplayMetricsAndroid::fontScale},
        {L"densityDpi", &DeviceInfoSpec_DisplayMetricsAndroid::densityDpi},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DeviceInfoSpec_DimensionsPayload*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"window", &DeviceInfoSpec_DimensionsPayload::window},
        {L"screen", &DeviceInfoSpec_DimensionsPayload::screen},
        {L"windowPhysicalPixels", &DeviceInfoSpec_DimensionsPayload::windowPhysicalPixels},
        {L"screenPhysicalPixels", &DeviceInfoSpec_DimensionsPayload::screenPhysicalPixels},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(DeviceInfoSpec_DeviceInfoConstants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"Dimensions", &DeviceInfoSpec_DeviceInfoConstants::Dimensions},
        {L"isIPhoneX_deprecated", &DeviceInfoSpec_DeviceInfoConstants::isIPhoneX_deprecated},
    };
    return fieldMap;
}

struct DeviceInfoSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DeviceInfoSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
