
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

struct JSCSamplingProfilerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double, std::string, std::string) noexcept>{0, L"operationComplete"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, JSCSamplingProfilerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "operationComplete",
          "    REACT_METHOD(operationComplete) void operationComplete(double token, std::string result, std::string error) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(operationComplete) static void operationComplete(double token, std::string result, std::string error) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
