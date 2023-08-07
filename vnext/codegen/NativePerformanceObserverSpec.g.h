
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

struct PerformanceObserverSpec_GetPendingEntriesResult {
    std::vector<PerformanceObserverSpec_RawPerformanceEntry> entries;
    double droppedEntriesCount;
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

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceObserverSpec_GetPendingEntriesResult*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"entries", &PerformanceObserverSpec_GetPendingEntriesResult::entries},
        {L"droppedEntriesCount", &PerformanceObserverSpec_GetPendingEntriesResult::droppedEntriesCount},
    };
    return fieldMap;
}

struct PerformanceObserverSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double) noexcept>{0, L"startReporting"},
      Method<void(double) noexcept>{1, L"stopReporting"},
      SyncMethod<PerformanceObserverSpec_GetPendingEntriesResult() noexcept>{2, L"popPendingEntries"},
      Method<void(Callback<>) noexcept>{3, L"setOnPerformanceEntryCallback"},
      Method<void(PerformanceObserverSpec_RawPerformanceEntry) noexcept>{4, L"logRawEntry"},
      SyncMethod<::React::JSValueArray() noexcept>{5, L"getEventCounts"},
      Method<void(double, double) noexcept>{6, L"setDurationThreshold"},
      Method<void(double, std::string) noexcept>{7, L"clearEntries"},
      SyncMethod<std::vector<PerformanceObserverSpec_RawPerformanceEntry>(double, std::string) noexcept>{8, L"getEntries"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PerformanceObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startReporting",
          "    REACT_METHOD(startReporting) void startReporting(double entryType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(startReporting) static void startReporting(double entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "stopReporting",
          "    REACT_METHOD(stopReporting) void stopReporting(double entryType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stopReporting) static void stopReporting(double entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "popPendingEntries",
          "    REACT_SYNC_METHOD(popPendingEntries) PerformanceObserverSpec_GetPendingEntriesResult popPendingEntries() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(popPendingEntries) static PerformanceObserverSpec_GetPendingEntriesResult popPendingEntries() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "setOnPerformanceEntryCallback",
          "    REACT_METHOD(setOnPerformanceEntryCallback) void setOnPerformanceEntryCallback(std::function<void()> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setOnPerformanceEntryCallback) static void setOnPerformanceEntryCallback(std::function<void()> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "logRawEntry",
          "    REACT_METHOD(logRawEntry) void logRawEntry(PerformanceObserverSpec_RawPerformanceEntry && entry) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(logRawEntry) static void logRawEntry(PerformanceObserverSpec_RawPerformanceEntry && entry) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getEventCounts",
          "    REACT_SYNC_METHOD(getEventCounts) ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEventCounts) static ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "setDurationThreshold",
          "    REACT_METHOD(setDurationThreshold) void setDurationThreshold(double entryType, double durationThreshold) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setDurationThreshold) static void setDurationThreshold(double entryType, double durationThreshold) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "clearEntries",
          "    REACT_METHOD(clearEntries) void clearEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearEntries) static void clearEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getEntries",
          "    REACT_SYNC_METHOD(getEntries) std::vector<PerformanceObserverSpec_RawPerformanceEntry> getEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEntries) static std::vector<PerformanceObserverSpec_RawPerformanceEntry> getEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
