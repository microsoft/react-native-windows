
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

struct PerformanceSpec_NativeMemoryInfo {

};

struct PerformanceSpec_ReactNativeStartupTiming {

};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceSpec_NativeMemoryInfo*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {

    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceSpec_ReactNativeStartupTiming*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {

    };
    return fieldMap;
}

struct PerformanceSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<double() noexcept>{0, L"now"},
      Method<void(std::string, double) noexcept>{1, L"mark"},
      Method<void(std::string, double, double, double, std::string, std::string) noexcept>{2, L"measure"},
      SyncMethod<::React::JSValue() noexcept>{3, L"getSimpleMemoryInfo"},
      SyncMethod<::React::JSValue() noexcept>{4, L"getReactNativeStartupTiming"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PerformanceSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "now",
          "    REACT_SYNC_METHOD(now) double now() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(now) static double now() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "mark",
          "    REACT_METHOD(mark) void mark(std::string name, double startTime) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(mark) static void mark(std::string name, double startTime) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "measure",
          "    REACT_METHOD(measure) void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getSimpleMemoryInfo",
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) static ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getReactNativeStartupTiming",
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) ::React::JSValue getReactNativeStartupTiming() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) static ::React::JSValue getReactNativeStartupTiming() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
