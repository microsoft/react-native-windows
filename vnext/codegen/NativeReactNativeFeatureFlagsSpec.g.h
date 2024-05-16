
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


struct ReactNativeFeatureFlagsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<bool() noexcept>{0, L"commonTestFlag"},
      SyncMethod<bool() noexcept>{1, L"batchRenderingUpdatesInEventLoop"},
      SyncMethod<bool() noexcept>{2, L"enableBackgroundExecutor"},
      SyncMethod<bool() noexcept>{3, L"enableCleanTextInputYogaNode"},
      SyncMethod<bool() noexcept>{4, L"enableCustomDrawOrderFabric"},
      SyncMethod<bool() noexcept>{5, L"enableMicrotasks"},
      SyncMethod<bool() noexcept>{6, L"enableMountHooksAndroid"},
      SyncMethod<bool() noexcept>{7, L"enableSpannableBuildingUnification"},
      SyncMethod<bool() noexcept>{8, L"enableSynchronousStateUpdates"},
      SyncMethod<bool() noexcept>{9, L"enableUIConsistency"},
      SyncMethod<bool() noexcept>{10, L"forceBatchingMountItemsOnAndroid"},
      SyncMethod<bool() noexcept>{11, L"inspectorEnableCxxInspectorPackagerConnection"},
      SyncMethod<bool() noexcept>{12, L"inspectorEnableModernCDPRegistry"},
      SyncMethod<bool() noexcept>{13, L"useModernRuntimeScheduler"},
      SyncMethod<bool() noexcept>{14, L"useNativeViewConfigsInBridgelessMode"},
      SyncMethod<bool() noexcept>{15, L"useStateAlignmentMechanism"},
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
          "batchRenderingUpdatesInEventLoop",
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) static bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "enableBackgroundExecutor",
          "    REACT_SYNC_METHOD(enableBackgroundExecutor) bool enableBackgroundExecutor() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBackgroundExecutor) static bool enableBackgroundExecutor() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "enableCleanTextInputYogaNode",
          "    REACT_SYNC_METHOD(enableCleanTextInputYogaNode) bool enableCleanTextInputYogaNode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCleanTextInputYogaNode) static bool enableCleanTextInputYogaNode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "enableCustomDrawOrderFabric",
          "    REACT_SYNC_METHOD(enableCustomDrawOrderFabric) bool enableCustomDrawOrderFabric() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCustomDrawOrderFabric) static bool enableCustomDrawOrderFabric() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "enableMicrotasks",
          "    REACT_SYNC_METHOD(enableMicrotasks) bool enableMicrotasks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMicrotasks) static bool enableMicrotasks() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "enableMountHooksAndroid",
          "    REACT_SYNC_METHOD(enableMountHooksAndroid) bool enableMountHooksAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMountHooksAndroid) static bool enableMountHooksAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "enableSpannableBuildingUnification",
          "    REACT_SYNC_METHOD(enableSpannableBuildingUnification) bool enableSpannableBuildingUnification() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSpannableBuildingUnification) static bool enableSpannableBuildingUnification() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableSynchronousStateUpdates",
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) static bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableUIConsistency",
          "    REACT_SYNC_METHOD(enableUIConsistency) bool enableUIConsistency() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableUIConsistency) static bool enableUIConsistency() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "forceBatchingMountItemsOnAndroid",
          "    REACT_SYNC_METHOD(forceBatchingMountItemsOnAndroid) bool forceBatchingMountItemsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(forceBatchingMountItemsOnAndroid) static bool forceBatchingMountItemsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "inspectorEnableCxxInspectorPackagerConnection",
          "    REACT_SYNC_METHOD(inspectorEnableCxxInspectorPackagerConnection) bool inspectorEnableCxxInspectorPackagerConnection() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(inspectorEnableCxxInspectorPackagerConnection) static bool inspectorEnableCxxInspectorPackagerConnection() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "inspectorEnableModernCDPRegistry",
          "    REACT_SYNC_METHOD(inspectorEnableModernCDPRegistry) bool inspectorEnableModernCDPRegistry() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(inspectorEnableModernCDPRegistry) static bool inspectorEnableModernCDPRegistry() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "useModernRuntimeScheduler",
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) static bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "useNativeViewConfigsInBridgelessMode",
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) static bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "useStateAlignmentMechanism",
          "    REACT_SYNC_METHOD(useStateAlignmentMechanism) bool useStateAlignmentMechanism() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useStateAlignmentMechanism) static bool useStateAlignmentMechanism() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
