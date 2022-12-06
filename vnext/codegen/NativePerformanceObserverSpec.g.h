
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

REACT_STRUCT(PerformanceObserverSpec_RawPerformanceEntry)
struct PerformanceObserverSpec_RawPerformanceEntry {
    REACT_FIELD(name)
    std::string name;
    REACT_FIELD(entryType)
    double entryType;
    REACT_FIELD(startTime)
    double startTime;
    REACT_FIELD(duration)
    double duration;
    REACT_FIELD(processingStart)
    std::optional<double> processingStart;
    REACT_FIELD(processingEnd)
    std::optional<double> processingEnd;
    REACT_FIELD(interactionId)
    std::optional<double> interactionId;
};

struct PerformanceObserverSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"startReporting"},
      Method<void(std::string) noexcept>{1, L"stopReporting"},
      SyncMethod<std::vector<PerformanceObserverSpec_RawPerformanceEntry>() noexcept>{2, L"getPendingEntries"},
      Method<void(Callback<>) noexcept>{3, L"setOnPerformanceEntryCallback"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PerformanceObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startReporting",
          "    REACT_METHOD(startReporting) void startReporting(std::string entryType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(startReporting) static void startReporting(std::string entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "stopReporting",
          "    REACT_METHOD(stopReporting) void stopReporting(std::string entryType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stopReporting) static void stopReporting(std::string entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getPendingEntries",
          "    REACT_SYNC_METHOD(getPendingEntries) std::vector<PerformanceObserverSpec_RawPerformanceEntry> getPendingEntries() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getPendingEntries) static std::vector<PerformanceObserverSpec_RawPerformanceEntry> getPendingEntries() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setOnPerformanceEntryCallback",
          "    REACT_METHOD(setOnPerformanceEntryCallback) void setOnPerformanceEntryCallback(std::function<void()> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setOnPerformanceEntryCallback) static void setOnPerformanceEntryCallback(std::function<void()> const & callback) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
