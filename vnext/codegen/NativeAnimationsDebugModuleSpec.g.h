
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

struct AnimationsDebugModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"startRecordingFps"},
      Method<void(double) noexcept>{1, L"stopRecordingFps"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AnimationsDebugModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startRecordingFps",
          "    REACT_METHOD(startRecordingFps) void startRecordingFps() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startRecordingFps) static void startRecordingFps() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "stopRecordingFps",
          "    REACT_METHOD(stopRecordingFps) void stopRecordingFps(double animationStopTimeMs) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopRecordingFps) static void stopRecordingFps(double animationStopTimeMs) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
