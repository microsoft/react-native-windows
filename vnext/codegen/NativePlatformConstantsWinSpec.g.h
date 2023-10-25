
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

struct PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion {
    double major;
    double minor;
    double patch;
    std::optional<std::string> prerelease;
};

struct PlatformConstantsWinSpec_PlatformConstantsWindows {
    bool isTesting;
    std::optional<bool> isDisableAnimations;
    PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion reactNativeVersion;
    double osVersion;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion::major},
        {L"minor", &PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion::minor},
        {L"patch", &PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion::patch},
        {L"prerelease", &PlatformConstantsWinSpec_PlatformConstantsWindows_reactNativeVersion::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWinSpec_PlatformConstantsWindows*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsWinSpec_PlatformConstantsWindows::isTesting},
        {L"isDisableAnimations", &PlatformConstantsWinSpec_PlatformConstantsWindows::isDisableAnimations},
        {L"reactNativeVersion", &PlatformConstantsWinSpec_PlatformConstantsWindows::reactNativeVersion},
        {L"osVersion", &PlatformConstantsWinSpec_PlatformConstantsWindows::osVersion},
    };
    return fieldMap;
}

struct PlatformConstantsWinSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsWinSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
