
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

struct HeapCaptureSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"captureHeap"},
      Method<void(std::string, std::string) noexcept>{1, L"captureComplete"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, HeapCaptureSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "captureHeap",
          "    REACT_METHOD(captureHeap) void captureHeap(std::string path) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(captureHeap) static void captureHeap(std::string path) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "captureComplete",
          "    REACT_METHOD(captureComplete) void captureComplete(std::string path, std::string error) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(captureComplete) static void captureComplete(std::string path, std::string error) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
