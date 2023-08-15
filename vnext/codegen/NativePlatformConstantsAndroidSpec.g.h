
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

struct PlatformConstantsAndroidSpec_Constants_reactNativeVersion {
    double major;
    double minor;
    double patch;
    std::optional<double> prerelease;
};

struct PlatformConstantsAndroidSpec_Constants {
    bool isTesting;
    PlatformConstantsAndroidSpec_Constants_reactNativeVersion reactNativeVersion;
    double Version;
    std::string Release;
    std::string Serial;
    std::string Fingerprint;
    std::string Model;
    std::optional<std::string> ServerHost;
    std::string uiMode;
    std::string Brand;
    std::string Manufacturer;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsAndroidSpec_Constants_reactNativeVersion*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsAndroidSpec_Constants_reactNativeVersion::major},
        {L"minor", &PlatformConstantsAndroidSpec_Constants_reactNativeVersion::minor},
        {L"patch", &PlatformConstantsAndroidSpec_Constants_reactNativeVersion::patch},
        {L"prerelease", &PlatformConstantsAndroidSpec_Constants_reactNativeVersion::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsAndroidSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsAndroidSpec_Constants::isTesting},
        {L"reactNativeVersion", &PlatformConstantsAndroidSpec_Constants::reactNativeVersion},
        {L"Version", &PlatformConstantsAndroidSpec_Constants::Version},
        {L"Release", &PlatformConstantsAndroidSpec_Constants::Release},
        {L"Serial", &PlatformConstantsAndroidSpec_Constants::Serial},
        {L"Fingerprint", &PlatformConstantsAndroidSpec_Constants::Fingerprint},
        {L"Model", &PlatformConstantsAndroidSpec_Constants::Model},
        {L"ServerHost", &PlatformConstantsAndroidSpec_Constants::ServerHost},
        {L"uiMode", &PlatformConstantsAndroidSpec_Constants::uiMode},
        {L"Brand", &PlatformConstantsAndroidSpec_Constants::Brand},
        {L"Manufacturer", &PlatformConstantsAndroidSpec_Constants::Manufacturer},
    };
    return fieldMap;
}

struct PlatformConstantsAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<PlatformConstantsAndroidSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      SyncMethod<std::string() noexcept>{0, L"getAndroidID"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, PlatformConstantsAndroidSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsAndroidSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "PlatformConstantsAndroidSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) PlatformConstantsAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static PlatformConstantsAndroidSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getAndroidID",
          "    REACT_SYNC_METHOD(getAndroidID) std::string getAndroidID() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getAndroidID) static std::string getAndroidID() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
