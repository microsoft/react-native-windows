
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

struct PerformanceObserverSpec_RawPerformanceEntry {
    std::string name;
    double entryType;
    double startTime;
    double duration;
    std::optional<double> processingStart;
    std::optional<double> processingEnd;
    std::optional<double> interactionId;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceObserverSpec_RawPerformanceEntry*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"name", &PerformanceObserverSpec_RawPerformanceEntry::name},
        {L"entryType", &PerformanceObserverSpec_RawPerformanceEntry::entryType},
        {L"startTime", &PerformanceObserverSpec_RawPerformanceEntry::startTime},
        {L"duration", &PerformanceObserverSpec_RawPerformanceEntry::duration},
        {L"processingStart", &PerformanceObserverSpec_RawPerformanceEntry::processingStart},
        {L"processingEnd", &PerformanceObserverSpec_RawPerformanceEntry::processingEnd},
        {L"interactionId", &PerformanceObserverSpec_RawPerformanceEntry::interactionId},
    };
    return fieldMap;
}

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
