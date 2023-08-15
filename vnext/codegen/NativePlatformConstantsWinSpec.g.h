
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

struct PlatformConstantsWinSpec_Constants_reactNativeVersion {
    double major;
    double minor;
    double patch;
    std::optional<std::string> prerelease;
};

struct PlatformConstantsWinSpec_Constants {
    bool isTesting;
    PlatformConstantsWinSpec_Constants_reactNativeVersion reactNativeVersion;
    double osVersion;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWinSpec_Constants_reactNativeVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsWinSpec_Constants_reactNativeVersion::major},
        {L"minor", &PlatformConstantsWinSpec_Constants_reactNativeVersion::minor},
        {L"patch", &PlatformConstantsWinSpec_Constants_reactNativeVersion::patch},
        {L"prerelease", &PlatformConstantsWinSpec_Constants_reactNativeVersion::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsWinSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsWinSpec_Constants::isTesting},
        {L"reactNativeVersion", &PlatformConstantsWinSpec_Constants::reactNativeVersion},
        {L"osVersion", &PlatformConstantsWinSpec_Constants::osVersion},
    };
    return fieldMap;
}

struct PlatformConstantsWinSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<PlatformConstantsWinSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, PlatformConstantsWinSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsWinSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "PlatformConstantsWinSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) PlatformConstantsWinSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static PlatformConstantsWinSpec_Constants GetConstants() noexcept {/*implementation*/}\n");


  }
};

} // namespace Microsoft::ReactNativeSpecs
