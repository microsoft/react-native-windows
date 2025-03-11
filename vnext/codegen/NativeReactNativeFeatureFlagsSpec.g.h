
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


struct ReactNativeFeatureFlagsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<bool() noexcept>{0, L"commonTestFlag"},
      SyncMethod<bool() noexcept>{1, L"androidEnablePendingFabricTransactions"},
      SyncMethod<bool() noexcept>{2, L"batchRenderingUpdatesInEventLoop"},
      SyncMethod<bool() noexcept>{3, L"destroyFabricSurfacesInReactInstanceManager"},
      SyncMethod<bool() noexcept>{4, L"enableBackgroundExecutor"},
      SyncMethod<bool() noexcept>{5, L"useModernRuntimeScheduler"},
      SyncMethod<bool() noexcept>{6, L"enableMicrotasks"},
      SyncMethod<bool() noexcept>{7, L"enableSpannableBuildingUnification"},
      SyncMethod<bool() noexcept>{8, L"enableCustomDrawOrderFabric"},
      SyncMethod<bool() noexcept>{9, L"enableFixForClippedSubviewsCrash"},
      SyncMethod<bool() noexcept>{10, L"inspectorEnableCxxInspectorPackagerConnection"},
      SyncMethod<bool() noexcept>{11, L"inspectorEnableModernCDPRegistry"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ReactNativeFeatureFlagsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "commonTestFlag",
          "    REACT_SYNC_METHOD(commonTestFlag) bool commonTestFlag() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(commonTestFlag) static bool commonTestFlag() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "androidEnablePendingFabricTransactions",
          "    REACT_SYNC_METHOD(androidEnablePendingFabricTransactions) bool androidEnablePendingFabricTransactions() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(androidEnablePendingFabricTransactions) static bool androidEnablePendingFabricTransactions() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "batchRenderingUpdatesInEventLoop",
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) static bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "destroyFabricSurfacesInReactInstanceManager",
          "    REACT_SYNC_METHOD(destroyFabricSurfacesInReactInstanceManager) bool destroyFabricSurfacesInReactInstanceManager() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(destroyFabricSurfacesInReactInstanceManager) static bool destroyFabricSurfacesInReactInstanceManager() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "enableBackgroundExecutor",
          "    REACT_SYNC_METHOD(enableBackgroundExecutor) bool enableBackgroundExecutor() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBackgroundExecutor) static bool enableBackgroundExecutor() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "useModernRuntimeScheduler",
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) static bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "enableMicrotasks",
          "    REACT_SYNC_METHOD(enableMicrotasks) bool enableMicrotasks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMicrotasks) static bool enableMicrotasks() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "enableSpannableBuildingUnification",
          "    REACT_SYNC_METHOD(enableSpannableBuildingUnification) bool enableSpannableBuildingUnification() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSpannableBuildingUnification) static bool enableSpannableBuildingUnification() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableCustomDrawOrderFabric",
          "    REACT_SYNC_METHOD(enableCustomDrawOrderFabric) bool enableCustomDrawOrderFabric() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCustomDrawOrderFabric) static bool enableCustomDrawOrderFabric() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableFixForClippedSubviewsCrash",
          "    REACT_SYNC_METHOD(enableFixForClippedSubviewsCrash) bool enableFixForClippedSubviewsCrash() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFixForClippedSubviewsCrash) static bool enableFixForClippedSubviewsCrash() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "inspectorEnableCxxInspectorPackagerConnection",
          "    REACT_SYNC_METHOD(inspectorEnableCxxInspectorPackagerConnection) bool inspectorEnableCxxInspectorPackagerConnection() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(inspectorEnableCxxInspectorPackagerConnection) static bool inspectorEnableCxxInspectorPackagerConnection() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "inspectorEnableModernCDPRegistry",
          "    REACT_SYNC_METHOD(inspectorEnableModernCDPRegistry) bool inspectorEnableModernCDPRegistry() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(inspectorEnableModernCDPRegistry) static bool inspectorEnableModernCDPRegistry() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
