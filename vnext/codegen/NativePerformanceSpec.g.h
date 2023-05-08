
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

REACT_STRUCT(PerformanceSpec_ReactNativeStartupTiming)
struct PerformanceSpec_ReactNativeStartupTiming {
    REACT_FIELD(startTime)
    double startTime;
    REACT_FIELD(endTime)
    double endTime;
    REACT_FIELD(executeJavaScriptBundleEntryPointStart)
    double executeJavaScriptBundleEntryPointStart;
    REACT_FIELD(executeJavaScriptBundleEntryPointEnd)
    double executeJavaScriptBundleEntryPointEnd;
};

struct PerformanceSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, double, double) noexcept>{0, L"mark"},
      Method<void(std::string, double, double, double, std::string, std::string) noexcept>{1, L"measure"},
      SyncMethod<::React::JSValue() noexcept>{2, L"getSimpleMemoryInfo"},
      SyncMethod<PerformanceSpec_ReactNativeStartupTiming() noexcept>{3, L"getReactNativeStartupTiming"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PerformanceSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "mark",
          "    REACT_METHOD(mark) void mark(std::string name, double startTime, double duration) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(mark) static void mark(std::string name, double startTime, double duration) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "measure",
          "    REACT_METHOD(measure) void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getSimpleMemoryInfo",
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) static ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getReactNativeStartupTiming",
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) PerformanceSpec_ReactNativeStartupTiming getReactNativeStartupTiming() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) static PerformanceSpec_ReactNativeStartupTiming getReactNativeStartupTiming() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
