
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {


struct DebuggerSessionObserverSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<bool() noexcept>{0, L"hasActiveSession"},
      SyncMethod<(Callback<bool>) noexcept>{1, L"subscribe"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DebuggerSessionObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "hasActiveSession",
          "    REACT_SYNC_METHOD(hasActiveSession) bool hasActiveSession() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(hasActiveSession) static bool hasActiveSession() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "subscribe",
          "    REACT_SYNC_METHOD(subscribe)  subscribe(std::function<void(bool)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(subscribe) static  subscribe(std::function<void(bool)> const & callback) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
