
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

REACT_STRUCT(PlatformConstantsAndroidSpec_Constants_reactNativeVersion)
struct PlatformConstantsAndroidSpec_Constants_reactNativeVersion {
    REACT_FIELD(major)
    double major;
    REACT_FIELD(minor)
    double minor;
    REACT_FIELD(patch)
    double patch;
    REACT_FIELD(prerelease)
    std::optional<double> prerelease;
};

REACT_STRUCT(PlatformConstantsAndroidSpec_Constants)
struct PlatformConstantsAndroidSpec_Constants {
    REACT_FIELD(isTesting)
    bool isTesting;
    REACT_FIELD(reactNativeVersion)
    PlatformConstantsAndroidSpec_Constants_reactNativeVersion reactNativeVersion;
    REACT_FIELD(Version)
    double Version;
    REACT_FIELD(Release)
    std::string Release;
    REACT_FIELD(Serial)
    std::string Serial;
    REACT_FIELD(Fingerprint)
    std::string Fingerprint;
    REACT_FIELD(Model)
    std::string Model;
    REACT_FIELD(ServerHost)
    std::optional<std::string> ServerHost;
    REACT_FIELD(uiMode)
    std::string uiMode;
    REACT_FIELD(Brand)
    std::string Brand;
    REACT_FIELD(Manufacturer)
    std::string Manufacturer;
};

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
          "    REACT_SYNC_METHOD(getAndroidID) std::string getAndroidID() noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getAndroidID) static std::string getAndroidID() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
