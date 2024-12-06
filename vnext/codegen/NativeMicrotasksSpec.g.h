
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


struct MicrotasksSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Callback<>) noexcept>{0, L"queueMicrotask"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, MicrotasksSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "queueMicrotask",
          "    REACT_METHOD(queueMicrotask) void queueMicrotask(std::function<void()> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(queueMicrotask) static void queueMicrotask(std::function<void()> const & callback) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
