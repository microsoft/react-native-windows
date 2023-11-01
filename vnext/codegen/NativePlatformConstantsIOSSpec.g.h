
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

struct PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion {
    double major;
    double minor;
    double patch;
    std::optional<double> prerelease;
};

struct PlatformConstantsIOSSpec_PlatformConstantsIOS {
    bool isTesting;
    std::optional<bool> isDisableAnimations;
    PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion reactNativeVersion;
    bool forceTouchAvailable;
    std::string osVersion;
    std::string systemName;
    std::string interfaceIdiom;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion::major},
        {L"minor", &PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion::minor},
        {L"patch", &PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion::patch},
        {L"prerelease", &PlatformConstantsIOSSpec_PlatformConstantsIOS_reactNativeVersion::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsIOSSpec_PlatformConstantsIOS*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsIOSSpec_PlatformConstantsIOS::isTesting},
        {L"isDisableAnimations", &PlatformConstantsIOSSpec_PlatformConstantsIOS::isDisableAnimations},
        {L"reactNativeVersion", &PlatformConstantsIOSSpec_PlatformConstantsIOS::reactNativeVersion},
        {L"forceTouchAvailable", &PlatformConstantsIOSSpec_PlatformConstantsIOS::forceTouchAvailable},
        {L"osVersion", &PlatformConstantsIOSSpec_PlatformConstantsIOS::osVersion},
        {L"systemName", &PlatformConstantsIOSSpec_PlatformConstantsIOS::systemName},
        {L"interfaceIdiom", &PlatformConstantsIOSSpec_PlatformConstantsIOS::interfaceIdiom},
    };
    return fieldMap;
}

struct PlatformConstantsIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsIOSSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
