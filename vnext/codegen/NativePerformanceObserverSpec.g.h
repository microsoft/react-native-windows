
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

struct PerformanceObserverSpec_PerformanceObserverInit {
    std::optional<std::vector<double>> entryTypes;
    std::optional<double> type;
    std::optional<bool> buffered;
    std::optional<double> durationThreshold;
};

struct PerformanceObserverSpec_RawPerformanceEntry {
    std::string name;
    double entryType;
    double startTime;
    double duration;
    std::optional<double> processingStart;
    std::optional<double> processingEnd;
    std::optional<double> interactionId;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(PerformanceObserverSpec_PerformanceObserverInit*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"entryTypes", &PerformanceObserverSpec_PerformanceObserverInit::entryTypes},
        {L"type", &PerformanceObserverSpec_PerformanceObserverInit::type},
        {L"buffered", &PerformanceObserverSpec_PerformanceObserverInit::buffered},
        {L"durationThreshold", &PerformanceObserverSpec_PerformanceObserverInit::durationThreshold},
    };
    return fieldMap;
}

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
      SyncMethod<::React::JSValueArray() noexcept>{0, L"getEventCounts"},
      SyncMethod<(Callback<>) noexcept>{1, L"createObserver"},
      SyncMethod<double() noexcept>{2, L"getDroppedEntriesCount"},
      Method<void(, PerformanceObserverSpec_PerformanceObserverInit) noexcept>{3, L"observe"},
      Method<void() noexcept>{4, L"disconnect"},
      SyncMethod<std::vector<PerformanceObserverSpec_RawPerformanceEntry>(, bool) noexcept>{5, L"takeRecords"},
      Method<void(double, std::string) noexcept>{6, L"clearEntries"},
      SyncMethod<std::vector<PerformanceObserverSpec_RawPerformanceEntry>(double, std::string) noexcept>{7, L"getEntries"},
      SyncMethod<std::vector<double>() noexcept>{8, L"getSupportedPerformanceEntryTypes"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, PerformanceObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getEventCounts",
          "    REACT_SYNC_METHOD(getEventCounts) ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEventCounts) static ::React::JSValueArray getEventCounts() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "createObserver",
          "    REACT_SYNC_METHOD(createObserver)  createObserver(std::function<void()> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(createObserver) static  createObserver(std::function<void()> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getDroppedEntriesCount",
          "    REACT_SYNC_METHOD(getDroppedEntriesCount) double getDroppedEntriesCount( observer) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getDroppedEntriesCount) static double getDroppedEntriesCount( observer) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "observe",
          "    REACT_METHOD(observe) void observe( observer, PerformanceObserverSpec_PerformanceObserverInit && options) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(observe) static void observe( observer, PerformanceObserverSpec_PerformanceObserverInit && options) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "disconnect",
          "    REACT_METHOD(disconnect) void disconnect( observer) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(disconnect) static void disconnect( observer) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "takeRecords",
          "    REACT_SYNC_METHOD(takeRecords) std::vector<PerformanceObserverSpec_RawPerformanceEntry> takeRecords( observer, bool sort) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(takeRecords) static std::vector<PerformanceObserverSpec_RawPerformanceEntry> takeRecords( observer, bool sort) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "clearEntries",
          "    REACT_METHOD(clearEntries) void clearEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearEntries) static void clearEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getEntries",
          "    REACT_SYNC_METHOD(getEntries) std::vector<PerformanceObserverSpec_RawPerformanceEntry> getEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEntries) static std::vector<PerformanceObserverSpec_RawPerformanceEntry> getEntries(double entryType, std::string entryName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getSupportedPerformanceEntryTypes",
          "    REACT_SYNC_METHOD(getSupportedPerformanceEntryTypes) std::vector<double> getSupportedPerformanceEntryTypes() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getSupportedPerformanceEntryTypes) static std::vector<double> getSupportedPerformanceEntryTypes() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
