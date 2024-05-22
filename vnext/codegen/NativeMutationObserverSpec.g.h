
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

struct MutationObserverSpec_NativeMutationObserverObserveOptions {
    double mutationObserverId;
     targetShadowNode;
    bool subtree;
};

struct MutationObserverSpec_NativeMutationRecord {
    double mutationObserverId;
     target;
    std::vector<> addedNodes;
    std::vector<> removedNodes;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(MutationObserverSpec_NativeMutationObserverObserveOptions*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"mutationObserverId", &MutationObserverSpec_NativeMutationObserverObserveOptions::mutationObserverId},
        {L"targetShadowNode", &MutationObserverSpec_NativeMutationObserverObserveOptions::targetShadowNode},
        {L"subtree", &MutationObserverSpec_NativeMutationObserverObserveOptions::subtree},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(MutationObserverSpec_NativeMutationRecord*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"mutationObserverId", &MutationObserverSpec_NativeMutationRecord::mutationObserverId},
        {L"target", &MutationObserverSpec_NativeMutationRecord::target},
        {L"addedNodes", &MutationObserverSpec_NativeMutationRecord::addedNodes},
        {L"removedNodes", &MutationObserverSpec_NativeMutationRecord::removedNodes},
    };
    return fieldMap;
}

struct MutationObserverSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(MutationObserverSpec_NativeMutationObserverObserveOptions) noexcept>{0, L"observe"},
      Method<void(double, ) noexcept>{1, L"unobserve"},
      Method<void(Callback<>, Callback<>) noexcept>{2, L"connect"},
      Method<void() noexcept>{3, L"disconnect"},
      SyncMethod<std::vector<MutationObserverSpec_NativeMutationRecord>() noexcept>{4, L"takeRecords"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, MutationObserverSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "observe",
          "    REACT_METHOD(observe) void observe(MutationObserverSpec_NativeMutationObserverObserveOptions && options) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(observe) static void observe(MutationObserverSpec_NativeMutationObserverObserveOptions && options) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "unobserve",
          "    REACT_METHOD(unobserve) void unobserve(double mutationObserverId,  targetShadowNode) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(unobserve) static void unobserve(double mutationObserverId,  targetShadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "connect",
          "    REACT_METHOD(connect) void connect(std::function<void()> const & notifyMutationObservers, std::function<void()> const & getPublicInstanceFromInstanceHandle) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(connect) static void connect(std::function<void()> const & notifyMutationObservers, std::function<void()> const & getPublicInstanceFromInstanceHandle) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "disconnect",
          "    REACT_METHOD(disconnect) void disconnect() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(disconnect) static void disconnect() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "takeRecords",
          "    REACT_SYNC_METHOD(takeRecords) std::vector<MutationObserverSpec_NativeMutationRecord> takeRecords() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(takeRecords) static std::vector<MutationObserverSpec_NativeMutationRecord> takeRecords() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
