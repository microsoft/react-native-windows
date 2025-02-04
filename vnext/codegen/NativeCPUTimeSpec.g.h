
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {


struct CPUTimeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<double() noexcept>{0, L"getCPUTimeNanos"},
      SyncMethod<bool() noexcept>{1, L"hasAccurateCPUTimeNanosForBenchmarks"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, CPUTimeSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getCPUTimeNanos",
          "    REACT_SYNC_METHOD(getCPUTimeNanos) double getCPUTimeNanos() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getCPUTimeNanos) static double getCPUTimeNanos() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "hasAccurateCPUTimeNanosForBenchmarks",
          "    REACT_SYNC_METHOD(hasAccurateCPUTimeNanosForBenchmarks) bool hasAccurateCPUTimeNanosForBenchmarks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(hasAccurateCPUTimeNanosForBenchmarks) static bool hasAccurateCPUTimeNanosForBenchmarks() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
