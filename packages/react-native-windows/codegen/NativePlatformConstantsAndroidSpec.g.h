
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
          "    REACT_SYNC_METHOD(getAndroidID) std::string getAndroidID() noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getAndroidID) static std::string getAndroidID() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
