
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

struct SoundManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"playTouchSound"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SoundManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "playTouchSound",
          "    REACT_METHOD(playTouchSound) void playTouchSound() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(playTouchSound) static void playTouchSound() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
