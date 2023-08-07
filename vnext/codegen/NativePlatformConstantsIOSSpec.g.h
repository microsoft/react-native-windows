
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

struct PlatformConstantsIOSSpec_Constants_reactNativeVersion {
    double major;
    double minor;
    double patch;
    std::optional<double> prerelease;
};

struct PlatformConstantsIOSSpec_Constants {
    bool isTesting;
    PlatformConstantsIOSSpec_Constants_reactNativeVersion reactNativeVersion;
    bool forceTouchAvailable;
    std::string osVersion;
    std::string systemName;
    std::string interfaceIdiom;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsIOSSpec_Constants_reactNativeVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsIOSSpec_Constants_reactNativeVersion::major},
        {L"minor", &PlatformConstantsIOSSpec_Constants_reactNativeVersion::minor},
        {L"patch", &PlatformConstantsIOSSpec_Constants_reactNativeVersion::patch},
        {L"prerelease", &PlatformConstantsIOSSpec_Constants_reactNativeVersion::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsIOSSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsIOSSpec_Constants::isTesting},
        {L"reactNativeVersion", &PlatformConstantsIOSSpec_Constants::reactNativeVersion},
        {L"forceTouchAvailable", &PlatformConstantsIOSSpec_Constants::forceTouchAvailable},
        {L"osVersion", &PlatformConstantsIOSSpec_Constants::osVersion},
        {L"systemName", &PlatformConstantsIOSSpec_Constants::systemName},
        {L"interfaceIdiom", &PlatformConstantsIOSSpec_Constants::interfaceIdiom},
    };
    return fieldMap;
}

struct PlatformConstantsIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<PlatformConstantsIOSSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, PlatformConstantsIOSSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsIOSSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "PlatformConstantsIOSSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) PlatformConstantsIOSSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static PlatformConstantsIOSSpec_Constants GetConstants() noexcept {/*implementation*/}\n");


  }
};

} // namespace Microsoft::ReactNativeSpecs
