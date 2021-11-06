
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

struct HeadlessJsTaskSupportSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double) noexcept>{0, L"notifyTaskFinished"},
      Method<void(double, Promise<::React::JSValue>) noexcept>{1, L"notifyTaskRetry"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, HeadlessJsTaskSupportSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "notifyTaskFinished",
          "    REACT_METHOD(notifyTaskFinished) void notifyTaskFinished(double taskId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(notifyTaskFinished) static void notifyTaskFinished(double taskId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "notifyTaskRetry",
          "    REACT_METHOD(notifyTaskRetry) void notifyTaskRetry(double taskId, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(notifyTaskRetry) static void notifyTaskRetry(double taskId, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
