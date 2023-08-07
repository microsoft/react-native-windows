
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

struct IntersectionObserverSpec_NativeIntersectionObserverObserveOptions {
    double intersectionObserverId;
     targetShadowNode;
    std::vector<double> thresholds;
};

struct IntersectionObserverSpec_NativeIntersectionObserverEntry {
    double intersectionObserverId;
     targetInstanceHandle;
    std::vector<double> targetRect;
    std::vector<double> rootRect;
    std::optional<std::vector<double>> intersectionRect;
    bool isIntersectingAboveThresholds;
    double time;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(IntersectionObserverSpec_NativeIntersectionObserverObserveOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"intersectionObserverId", &IntersectionObserverSpec_NativeIntersectionObserverObserveOptions::intersectionObserverId},
        {L"targetShadowNode", &IntersectionObserverSpec_NativeIntersectionObserverObserveOptions::targetShadowNode},
        {L"thresholds", &IntersectionObserverSpec_NativeIntersectionObserverObserveOptions::thresholds},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(IntersectionObserverSpec_NativeIntersectionObserverEntry*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"intersectionObserverId", &IntersectionObserverSpec_NativeIntersectionObserverEntry::intersectionObserverId},
        {L"targetInstanceHandle", &IntersectionObserverSpec_NativeIntersectionObserverEntry::targetInstanceHandle},
        {L"targetRect", &IntersectionObserverSpec_NativeIntersectionObserverEntry::targetRect},
        {L"rootRect", &IntersectionObserverSpec_NativeIntersectionObserverEntry::rootRect},
        {L"intersectionRect", &IntersectionObserverSpec_NativeIntersectionObserverEntry::intersectionRect},
        {L"isIntersectingAboveThresholds", &IntersectionObserverSpec_NativeIntersectionObserverEntry::isIntersectingAboveThresholds},
        {L"time", &IntersectionObserverSpec_NativeIntersectionObserverEntry::time},
    };
    return fieldMap;
}

struct IntersectionObserverSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(IntersectionObserverSpec_NativeIntersectionObserverObserveOptions) noexcept>{0, L"observe"},
      Method<void(double, ) noexcept>{1, L"unobserve"},
      Method<void(Callback<>) noexcept>{2, L"connect"},
      Method<void() noexcept>{3, L"disconnect"},
      SyncMethod<std::vector<IntersectionObserverSpec_NativeIntersectionObserverEntry>() noexcept>{4, L"takeRecords"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, IntersectionObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "observe",
          "    REACT_METHOD(observe) void observe(IntersectionObserverSpec_NativeIntersectionObserverObserveOptions && options) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(observe) static void observe(IntersectionObserverSpec_NativeIntersectionObserverObserveOptions && options) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "unobserve",
          "    REACT_METHOD(unobserve) void unobserve(double intersectionObserverId,  targetShadowNode) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(unobserve) static void unobserve(double intersectionObserverId,  targetShadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "connect",
          "    REACT_METHOD(connect) void connect(std::function<void()> const & notifyIntersectionObserversCallback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(connect) static void connect(std::function<void()> const & notifyIntersectionObserversCallback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "disconnect",
          "    REACT_METHOD(disconnect) void disconnect() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(disconnect) static void disconnect() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "takeRecords",
          "    REACT_SYNC_METHOD(takeRecords) std::vector<IntersectionObserverSpec_NativeIntersectionObserverEntry> takeRecords() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(takeRecords) static std::vector<IntersectionObserverSpec_NativeIntersectionObserverEntry> takeRecords() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
