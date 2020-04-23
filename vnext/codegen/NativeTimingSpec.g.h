
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

struct TimingSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double, double, double, bool) noexcept>{0, L"createTimer"},
      Method<void(double) noexcept>{1, L"deleteTimer"},
      Method<void(bool) noexcept>{2, L"setSendIdleEvents"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, TimingSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "createTimer",
          "    REACT_METHOD(createTimer) void createTimer(double callbackID, double duration, double jsSchedulingTime, bool repeats) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createTimer) static void createTimer(double callbackID, double duration, double jsSchedulingTime, bool repeats) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "deleteTimer",
          "    REACT_METHOD(deleteTimer) void deleteTimer(double timerID) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(deleteTimer) static void deleteTimer(double timerID) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "setSendIdleEvents",
          "    REACT_METHOD(setSendIdleEvents) void setSendIdleEvents(bool sendIdleEvents) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setSendIdleEvents) static void setSendIdleEvents(bool sendIdleEvents) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
