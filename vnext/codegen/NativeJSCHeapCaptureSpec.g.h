
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

struct JSCHeapCaptureSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, std::optional<std::string>) noexcept>{0, L"captureComplete"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, JSCHeapCaptureSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "captureComplete",
          "    REACT_METHOD(captureComplete) void captureComplete(std::string path, std::optional<std::string> error) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(captureComplete) static void captureComplete(std::string path, std::optional<std::string> error) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
