
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

struct PlatformConstantsAndroidSpec_ReactNativeVersionAndroid {
    double major;
    double minor;
    double patch;
    std::optional<double> prerelease;
};

struct PlatformConstantsAndroidSpec_PlatformConstantsAndroid {
    bool isTesting;
    std::optional<bool> isDisableAnimations;
    PlatformConstantsAndroidSpec_ReactNativeVersionAndroid reactNativeVersion;
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


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsAndroidSpec_ReactNativeVersionAndroid*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"major", &PlatformConstantsAndroidSpec_ReactNativeVersionAndroid::major},
        {L"minor", &PlatformConstantsAndroidSpec_ReactNativeVersionAndroid::minor},
        {L"patch", &PlatformConstantsAndroidSpec_ReactNativeVersionAndroid::patch},
        {L"prerelease", &PlatformConstantsAndroidSpec_ReactNativeVersionAndroid::prerelease},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PlatformConstantsAndroidSpec_PlatformConstantsAndroid*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"isTesting", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::isTesting},
        {L"isDisableAnimations", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::isDisableAnimations},
        {L"reactNativeVersion", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::reactNativeVersion},
        {L"Version", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Version},
        {L"Release", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Release},
        {L"Serial", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Serial},
        {L"Fingerprint", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Fingerprint},
        {L"Model", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Model},
        {L"ServerHost", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::ServerHost},
        {L"uiMode", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::uiMode},
        {L"Brand", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Brand},
        {L"Manufacturer", &PlatformConstantsAndroidSpec_PlatformConstantsAndroid::Manufacturer},
    };
    return fieldMap;
}

struct PlatformConstantsAndroidSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<std::string() noexcept>{0, L"getAndroidID"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PlatformConstantsAndroidSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getAndroidID",
          "    REACT_SYNC_METHOD(getAndroidID) std::string getAndroidID() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getAndroidID) static std::string getAndroidID() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
