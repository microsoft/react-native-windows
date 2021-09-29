
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

REACT_STRUCT(PlatformConstantsIOSSpec_Constants_reactNativeVersion)
struct PlatformConstantsIOSSpec_Constants_reactNativeVersion {
    REACT_FIELD(major)
    double major;
    REACT_FIELD(minor)
    double minor;
    REACT_FIELD(patch)
    double patch;
    REACT_FIELD(prerelease)
    std::optional<double> prerelease;
};

REACT_STRUCT(PlatformConstantsIOSSpec_Constants)
struct PlatformConstantsIOSSpec_Constants {
    REACT_FIELD(isTesting)
    bool isTesting;
    REACT_FIELD(reactNativeVersion)
    PlatformConstantsIOSSpec_Constants_reactNativeVersion reactNativeVersion;
    REACT_FIELD(forceTouchAvailable)
    bool forceTouchAvailable;
    REACT_FIELD(osVersion)
    std::string osVersion;
    REACT_FIELD(systemName)
    std::string systemName;
    REACT_FIELD(interfaceIdiom)
    std::string interfaceIdiom;
};

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
