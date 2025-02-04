
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

struct IdleCallbacksSpec_IdleDeadline {
    bool didTimeout;
     timeRemaining;
};

struct IdleCallbacksSpec_RequestIdleCallbackOptions {
    std::optional<double> timeout;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(IdleCallbacksSpec_IdleDeadline*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"didTimeout", &IdleCallbacksSpec_IdleDeadline::didTimeout},
        {L"timeRemaining", &IdleCallbacksSpec_IdleDeadline::timeRemaining},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(IdleCallbacksSpec_RequestIdleCallbackOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"timeout", &IdleCallbacksSpec_RequestIdleCallbackOptions::timeout},
    };
    return fieldMap;
}

struct IdleCallbacksSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<(Callback<IdleCallbacksSpec_IdleDeadline>, IdleCallbacksSpec_RequestIdleCallbackOptions) noexcept>{0, L"requestIdleCallback"},
      Method<void() noexcept>{1, L"cancelIdleCallback"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, IdleCallbacksSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "requestIdleCallback",
          "    REACT_SYNC_METHOD(requestIdleCallback)  requestIdleCallback(std::function<void(IdleCallbacksSpec_IdleDeadline const &)> const & callback, IdleCallbacksSpec_RequestIdleCallbackOptions && options) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(requestIdleCallback) static  requestIdleCallback(std::function<void(IdleCallbacksSpec_IdleDeadline const &)> const & callback, IdleCallbacksSpec_RequestIdleCallbackOptions && options) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "cancelIdleCallback",
          "    REACT_METHOD(cancelIdleCallback) void cancelIdleCallback( handle) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(cancelIdleCallback) static void cancelIdleCallback( handle) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
