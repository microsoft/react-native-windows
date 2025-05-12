
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

struct FantomSpec_RenderFormatOptions {
    bool includeRoot;
    bool includeLayoutMetrics;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(FantomSpec_RenderFormatOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"includeRoot", &FantomSpec_RenderFormatOptions::includeRoot},
        {L"includeLayoutMetrics", &FantomSpec_RenderFormatOptions::includeLayoutMetrics},
    };
    return fieldMap;
}

struct FantomSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double, double, double, double) noexcept>{0, L"startSurface"},
      Method<void(double) noexcept>{1, L"stopSurface"},
      Method<void(, std::string, ) noexcept>{2, L"dispatchNativeEvent"},
      SyncMethod<std::vector<std::string>(double) noexcept>{3, L"getMountingManagerLogs"},
      Method<void() noexcept>{4, L"flushMessageQueue"},
      Method<void() noexcept>{5, L"flushEventQueue"},
      SyncMethod<std::string(double, FantomSpec_RenderFormatOptions) noexcept>{6, L"getRenderedOutput"},
      Method<void(std::string) noexcept>{7, L"reportTestSuiteResultsJSON"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, FantomSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startSurface",
          "    REACT_METHOD(startSurface) void startSurface(double surfaceId, double viewportWidth, double viewportHeight, double devicePixelRatio) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(startSurface) static void startSurface(double surfaceId, double viewportWidth, double viewportHeight, double devicePixelRatio) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "stopSurface",
          "    REACT_METHOD(stopSurface) void stopSurface(double surfaceId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stopSurface) static void stopSurface(double surfaceId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "dispatchNativeEvent",
          "    REACT_METHOD(dispatchNativeEvent) void dispatchNativeEvent( shadowNode, std::string type,  payload) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(dispatchNativeEvent) static void dispatchNativeEvent( shadowNode, std::string type,  payload) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getMountingManagerLogs",
          "    REACT_SYNC_METHOD(getMountingManagerLogs) std::vector<std::string> getMountingManagerLogs(double surfaceId) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getMountingManagerLogs) static std::vector<std::string> getMountingManagerLogs(double surfaceId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "flushMessageQueue",
          "    REACT_METHOD(flushMessageQueue) void flushMessageQueue() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(flushMessageQueue) static void flushMessageQueue() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "flushEventQueue",
          "    REACT_METHOD(flushEventQueue) void flushEventQueue() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(flushEventQueue) static void flushEventQueue() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getRenderedOutput",
          "    REACT_SYNC_METHOD(getRenderedOutput) std::string getRenderedOutput(double surfaceId, FantomSpec_RenderFormatOptions && config) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getRenderedOutput) static std::string getRenderedOutput(double surfaceId, FantomSpec_RenderFormatOptions && config) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "reportTestSuiteResultsJSON",
          "    REACT_METHOD(reportTestSuiteResultsJSON) void reportTestSuiteResultsJSON(std::string results) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(reportTestSuiteResultsJSON) static void reportTestSuiteResultsJSON(std::string results) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
