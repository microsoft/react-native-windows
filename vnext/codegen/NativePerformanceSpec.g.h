
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

struct PerformanceSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, double, double) noexcept>{0, L"mark"},
      Method<void(std::string) noexcept>{1, L"clearMarks"},
      Method<void(std::string, double, double, double, std::string, std::string) noexcept>{2, L"measure"},
      Method<void(std::string) noexcept>{3, L"clearMeasures"},
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
          "clearMarks",
          "    REACT_METHOD(clearMarks) void clearMarks(std::string markName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearMarks) static void clearMarks(std::string markName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "measure",
          "    REACT_METHOD(measure) void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "clearMeasures",
          "    REACT_METHOD(clearMeasures) void clearMeasures(std::string measureName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearMeasures) static void clearMeasures(std::string measureName) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
