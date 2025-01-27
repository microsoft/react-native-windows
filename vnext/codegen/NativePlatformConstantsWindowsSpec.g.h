
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct PlatformConstantsWindowsSpec_ReactNativeVersionAndroid {
    double major;
    double minor;
    double patch;
    std::optional<std::string> prerelease;
};

struct PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion {
    double major;
    double minor;
    double patch;
};

struct PlatformConstantsWindowsSpec_PlatformConstantsWindows {
    bool isTesting;
    std::optional<bool> isDisableAnimations;
    PlatformConstantsWindowsSpec_ReactNativeVersionAndroid reactNativeVersion;
    PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion reactNativeWindowsVersion;
    double osVersion;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWindowsSpec_ReactNativeVersionAndroid*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsWindowsSpec_ReactNativeVersionAndroid::major},
        {L"minor", &PlatformConstantsWindowsSpec_ReactNativeVersionAndroid::minor},
        {L"patch", &PlatformConstantsWindowsSpec_ReactNativeVersionAndroid::patch},
        {L"prerelease", &PlatformConstantsWindowsSpec_ReactNativeVersionAndroid::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion::major},
        {L"minor", &PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion::minor},
        {L"patch", &PlatformConstantsWindowsSpec_PlatformConstantsWindows_reactNativeWindowsVersion::patch},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWindowsSpec_PlatformConstantsWindows*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsWindowsSpec_PlatformConstantsWindows::isTesting},
        {L"isDisableAnimations", &PlatformConstantsWindowsSpec_PlatformConstantsWindows::isDisableAnimations},
        {L"reactNativeVersion", &PlatformConstantsWindowsSpec_PlatformConstantsWindows::reactNativeVersion},
        {L"reactNativeWindowsVersion", &PlatformConstantsWindowsSpec_PlatformConstantsWindows::reactNativeWindowsVersion},
        {L"osVersion", &PlatformConstantsWindowsSpec_PlatformConstantsWindows::osVersion},
    };
    return fieldMap;
}

struct PlatformConstantsWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsWindowsSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
