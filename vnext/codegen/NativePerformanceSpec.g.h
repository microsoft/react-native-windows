
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

struct PerformanceSpec_PerformanceObserverInit {
    std::optional<std::vector<double>> entryTypes;
    std::optional<double> type;
    std::optional<bool> buffered;
    std::optional<double> durationThreshold;
};

struct PerformanceSpec_RawPerformanceEntry {
    std::string name;
    double entryType;
    double startTime;
    double duration;
    std::optional<double> processingStart;
    std::optional<double> processingEnd;
    std::optional<double> interactionId;
    std::optional<double> fetchStart;
    std::optional<double> requestStart;
    std::optional<double> connectStart;
    std::optional<double> connectEnd;
    std::optional<double> responseStart;
    std::optional<double> responseEnd;
    std::optional<double> responseStatus;
};

struct PerformanceSpec_ReactNativeStartupTiming {

};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceSpec_NativeMemoryInfo*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {

    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceSpec_PerformanceObserverInit*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"entryTypes", &PerformanceSpec_PerformanceObserverInit::entryTypes},
        {L"type", &PerformanceSpec_PerformanceObserverInit::type},
        {L"buffered", &PerformanceSpec_PerformanceObserverInit::buffered},
        {L"durationThreshold", &PerformanceSpec_PerformanceObserverInit::durationThreshold},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceSpec_RawPerformanceEntry*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"name", &PerformanceSpec_RawPerformanceEntry::name},
        {L"entryType", &PerformanceSpec_RawPerformanceEntry::entryType},
        {L"startTime", &PerformanceSpec_RawPerformanceEntry::startTime},
        {L"duration", &PerformanceSpec_RawPerformanceEntry::duration},
        {L"processingStart", &PerformanceSpec_RawPerformanceEntry::processingStart},
        {L"processingEnd", &PerformanceSpec_RawPerformanceEntry::processingEnd},
        {L"interactionId", &PerformanceSpec_RawPerformanceEntry::interactionId},
        {L"fetchStart", &PerformanceSpec_RawPerformanceEntry::fetchStart},
        {L"requestStart", &PerformanceSpec_RawPerformanceEntry::requestStart},
        {L"connectStart", &PerformanceSpec_RawPerformanceEntry::connectStart},
        {L"connectEnd", &PerformanceSpec_RawPerformanceEntry::connectEnd},
        {L"responseStart", &PerformanceSpec_RawPerformanceEntry::responseStart},
        {L"responseEnd", &PerformanceSpec_RawPerformanceEntry::responseEnd},
        {L"responseStatus", &PerformanceSpec_RawPerformanceEntry::responseStatus},
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
      SyncMethod<double(std::string, double) noexcept>{1, L"markWithResult"},
      SyncMethod<std::vector<double>(std::string, double, double, double, std::string, std::string) noexcept>{2, L"measureWithResult"},
      Method<void(std::string) noexcept>{3, L"clearMarks"},
      Method<void(std::string) noexcept>{4, L"clearMeasures"},
      SyncMethod<std::vector<PerformanceSpec_RawPerformanceEntry>() noexcept>{5, L"getEntries"},
      SyncMethod<std::vector<PerformanceSpec_RawPerformanceEntry>(std::string, std::optional<double>) noexcept>{6, L"getEntriesByName"},
      SyncMethod<std::vector<PerformanceSpec_RawPerformanceEntry>(double) noexcept>{7, L"getEntriesByType"},
      SyncMethod<::React::JSValueArray() noexcept>{8, L"getEventCounts"},
      SyncMethod<::React::JSValue() noexcept>{9, L"getSimpleMemoryInfo"},
      SyncMethod<::React::JSValue() noexcept>{10, L"getReactNativeStartupTiming"},
      SyncMethod<(Callback<>) noexcept>{11, L"createObserver"},
      SyncMethod<double() noexcept>{12, L"getDroppedEntriesCount"},
      Method<void(, PerformanceSpec_PerformanceObserverInit) noexcept>{13, L"observe"},
      Method<void() noexcept>{14, L"disconnect"},
      SyncMethod<std::vector<PerformanceSpec_RawPerformanceEntry>(, bool) noexcept>{15, L"takeRecords"},
      SyncMethod<std::vector<double>() noexcept>{16, L"getSupportedPerformanceEntryTypes"},
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
          "markWithResult",
          "    REACT_SYNC_METHOD(markWithResult) double markWithResult(std::string name, double startTime) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(markWithResult) static double markWithResult(std::string name, double startTime) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "measureWithResult",
          "    REACT_SYNC_METHOD(measureWithResult) std::vector<double> measureWithResult(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(measureWithResult) static std::vector<double> measureWithResult(std::string name, double startTime, double endTime, double duration, std::string startMark, std::string endMark) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "clearMarks",
          "    REACT_METHOD(clearMarks) void clearMarks(std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearMarks) static void clearMarks(std::string entryName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "clearMeasures",
          "    REACT_METHOD(clearMeasures) void clearMeasures(std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearMeasures) static void clearMeasures(std::string entryName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getEntries",
          "    REACT_SYNC_METHOD(getEntries) std::vector<PerformanceSpec_RawPerformanceEntry> getEntries() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEntries) static std::vector<PerformanceSpec_RawPerformanceEntry> getEntries() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getEntriesByName",
          "    REACT_SYNC_METHOD(getEntriesByName) std::vector<PerformanceSpec_RawPerformanceEntry> getEntriesByName(std::string entryName, std::optional<double> entryType) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEntriesByName) static std::vector<PerformanceSpec_RawPerformanceEntry> getEntriesByName(std::string entryName, std::optional<double> entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getEntriesByType",
          "    REACT_SYNC_METHOD(getEntriesByType) std::vector<PerformanceSpec_RawPerformanceEntry> getEntriesByType(double entryType) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEntriesByType) static std::vector<PerformanceSpec_RawPerformanceEntry> getEntriesByType(double entryType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getEventCounts",
          "    REACT_SYNC_METHOD(getEventCounts) ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEventCounts) static ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "getSimpleMemoryInfo",
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getSimpleMemoryInfo) static ::React::JSValue getSimpleMemoryInfo() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getReactNativeStartupTiming",
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) ::React::JSValue getReactNativeStartupTiming() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getReactNativeStartupTiming) static ::React::JSValue getReactNativeStartupTiming() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "createObserver",
          "    REACT_SYNC_METHOD(createObserver)  createObserver(std::function<void()> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(createObserver) static  createObserver(std::function<void()> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "getDroppedEntriesCount",
          "    REACT_SYNC_METHOD(getDroppedEntriesCount) double getDroppedEntriesCount( observer) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getDroppedEntriesCount) static double getDroppedEntriesCount( observer) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "observe",
          "    REACT_METHOD(observe) void observe( observer, PerformanceSpec_PerformanceObserverInit && options) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(observe) static void observe( observer, PerformanceSpec_PerformanceObserverInit && options) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "disconnect",
          "    REACT_METHOD(disconnect) void disconnect( observer) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(disconnect) static void disconnect( observer) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "takeRecords",
          "    REACT_SYNC_METHOD(takeRecords) std::vector<PerformanceSpec_RawPerformanceEntry> takeRecords( observer, bool sort) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(takeRecords) static std::vector<PerformanceSpec_RawPerformanceEntry> takeRecords( observer, bool sort) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "getSupportedPerformanceEntryTypes",
          "    REACT_SYNC_METHOD(getSupportedPerformanceEntryTypes) std::vector<double> getSupportedPerformanceEntryTypes() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getSupportedPerformanceEntryTypes) static std::vector<double> getSupportedPerformanceEntryTypes() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
