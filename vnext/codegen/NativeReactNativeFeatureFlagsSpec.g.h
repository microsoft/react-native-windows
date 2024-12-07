
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
      SyncMethod<bool() noexcept>{1, L"commonTestFlagWithoutNativeImplementation"},
      SyncMethod<bool() noexcept>{2, L"completeReactInstanceCreationOnBgThreadOnAndroid"},
      SyncMethod<bool() noexcept>{3, L"disableEventLoopOnBridgeless"},
      SyncMethod<bool() noexcept>{4, L"disableMountItemReorderingAndroid"},
      SyncMethod<bool() noexcept>{5, L"enableAlignItemsBaselineOnFabricIOS"},
      SyncMethod<bool() noexcept>{6, L"enableAndroidLineHeightCentering"},
      SyncMethod<bool() noexcept>{7, L"enableBridgelessArchitecture"},
      SyncMethod<bool() noexcept>{8, L"enableCppPropsIteratorSetter"},
      SyncMethod<bool() noexcept>{9, L"enableDeletionOfUnmountedViews"},
      SyncMethod<bool() noexcept>{10, L"enableEagerRootViewAttachment"},
      SyncMethod<bool() noexcept>{11, L"enableEventEmitterRetentionDuringGesturesOnAndroid"},
      SyncMethod<bool() noexcept>{12, L"enableFabricLogs"},
      SyncMethod<bool() noexcept>{13, L"enableFabricRenderer"},
      SyncMethod<bool() noexcept>{14, L"enableFabricRendererExclusively"},
      SyncMethod<bool() noexcept>{15, L"enableFixForViewCommandRace"},
      SyncMethod<bool() noexcept>{16, L"enableGranularShadowTreeStateReconciliation"},
      SyncMethod<bool() noexcept>{17, L"enableIOSViewClipToPaddingBox"},
      SyncMethod<bool() noexcept>{18, L"enableLayoutAnimationsOnAndroid"},
      SyncMethod<bool() noexcept>{19, L"enableLayoutAnimationsOnIOS"},
      SyncMethod<bool() noexcept>{20, L"enableLongTaskAPI"},
      SyncMethod<bool() noexcept>{21, L"enableNewBackgroundAndBorderDrawables"},
      SyncMethod<bool() noexcept>{22, L"enablePreciseSchedulingForPremountItemsOnAndroid"},
      SyncMethod<bool() noexcept>{23, L"enablePropsUpdateReconciliationAndroid"},
      SyncMethod<bool() noexcept>{24, L"enableReportEventPaintTime"},
      SyncMethod<bool() noexcept>{25, L"enableSynchronousStateUpdates"},
      SyncMethod<bool() noexcept>{26, L"enableUIConsistency"},
      SyncMethod<bool() noexcept>{27, L"enableViewRecycling"},
      SyncMethod<bool() noexcept>{28, L"excludeYogaFromRawProps"},
      SyncMethod<bool() noexcept>{29, L"fixMappingOfEventPrioritiesBetweenFabricAndReact"},
      SyncMethod<bool() noexcept>{30, L"fixMountingCoordinatorReportedPendingTransactionsOnAndroid"},
      SyncMethod<bool() noexcept>{31, L"fuseboxEnabledDebug"},
      SyncMethod<bool() noexcept>{32, L"fuseboxEnabledRelease"},
      SyncMethod<bool() noexcept>{33, L"initEagerTurboModulesOnNativeModulesQueueAndroid"},
      SyncMethod<bool() noexcept>{34, L"lazyAnimationCallbacks"},
      SyncMethod<bool() noexcept>{35, L"loadVectorDrawablesOnImages"},
      SyncMethod<bool() noexcept>{36, L"setAndroidLayoutDirection"},
      SyncMethod<bool() noexcept>{37, L"traceTurboModulePromiseRejectionsOnAndroid"},
      SyncMethod<bool() noexcept>{38, L"useAlwaysAvailableJSErrorHandling"},
      SyncMethod<bool() noexcept>{39, L"useFabricInterop"},
      SyncMethod<bool() noexcept>{40, L"useImmediateExecutorInAndroidBridgeless"},
      SyncMethod<bool() noexcept>{41, L"useNativeViewConfigsInBridgelessMode"},
      SyncMethod<bool() noexcept>{42, L"useOptimisedViewPreallocationOnAndroid"},
      SyncMethod<bool() noexcept>{43, L"useOptimizedEventBatchingOnAndroid"},
      SyncMethod<bool() noexcept>{44, L"useRuntimeShadowNodeReferenceUpdate"},
      SyncMethod<bool() noexcept>{45, L"useTurboModuleInterop"},
      SyncMethod<bool() noexcept>{46, L"useTurboModules"},
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
          "commonTestFlagWithoutNativeImplementation",
          "    REACT_SYNC_METHOD(commonTestFlagWithoutNativeImplementation) bool commonTestFlagWithoutNativeImplementation() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(commonTestFlagWithoutNativeImplementation) static bool commonTestFlagWithoutNativeImplementation() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "completeReactInstanceCreationOnBgThreadOnAndroid",
          "    REACT_SYNC_METHOD(completeReactInstanceCreationOnBgThreadOnAndroid) bool completeReactInstanceCreationOnBgThreadOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(completeReactInstanceCreationOnBgThreadOnAndroid) static bool completeReactInstanceCreationOnBgThreadOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "disableEventLoopOnBridgeless",
          "    REACT_SYNC_METHOD(disableEventLoopOnBridgeless) bool disableEventLoopOnBridgeless() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(disableEventLoopOnBridgeless) static bool disableEventLoopOnBridgeless() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "disableMountItemReorderingAndroid",
          "    REACT_SYNC_METHOD(disableMountItemReorderingAndroid) bool disableMountItemReorderingAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(disableMountItemReorderingAndroid) static bool disableMountItemReorderingAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "enableAlignItemsBaselineOnFabricIOS",
          "    REACT_SYNC_METHOD(enableAlignItemsBaselineOnFabricIOS) bool enableAlignItemsBaselineOnFabricIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAlignItemsBaselineOnFabricIOS) static bool enableAlignItemsBaselineOnFabricIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "enableAndroidLineHeightCentering",
          "    REACT_SYNC_METHOD(enableAndroidLineHeightCentering) bool enableAndroidLineHeightCentering() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAndroidLineHeightCentering) static bool enableAndroidLineHeightCentering() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "enableBridgelessArchitecture",
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) static bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableCppPropsIteratorSetter",
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) static bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableDeletionOfUnmountedViews",
          "    REACT_SYNC_METHOD(enableDeletionOfUnmountedViews) bool enableDeletionOfUnmountedViews() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDeletionOfUnmountedViews) static bool enableDeletionOfUnmountedViews() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "enableEagerRootViewAttachment",
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) static bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "enableEventEmitterRetentionDuringGesturesOnAndroid",
          "    REACT_SYNC_METHOD(enableEventEmitterRetentionDuringGesturesOnAndroid) bool enableEventEmitterRetentionDuringGesturesOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEventEmitterRetentionDuringGesturesOnAndroid) static bool enableEventEmitterRetentionDuringGesturesOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "enableFabricLogs",
          "    REACT_SYNC_METHOD(enableFabricLogs) bool enableFabricLogs() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricLogs) static bool enableFabricLogs() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "enableFabricRenderer",
          "    REACT_SYNC_METHOD(enableFabricRenderer) bool enableFabricRenderer() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricRenderer) static bool enableFabricRenderer() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "enableFabricRendererExclusively",
          "    REACT_SYNC_METHOD(enableFabricRendererExclusively) bool enableFabricRendererExclusively() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricRendererExclusively) static bool enableFabricRendererExclusively() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "enableFixForViewCommandRace",
          "    REACT_SYNC_METHOD(enableFixForViewCommandRace) bool enableFixForViewCommandRace() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFixForViewCommandRace) static bool enableFixForViewCommandRace() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "enableGranularShadowTreeStateReconciliation",
          "    REACT_SYNC_METHOD(enableGranularShadowTreeStateReconciliation) bool enableGranularShadowTreeStateReconciliation() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableGranularShadowTreeStateReconciliation) static bool enableGranularShadowTreeStateReconciliation() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "enableIOSViewClipToPaddingBox",
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) static bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "enableLayoutAnimationsOnAndroid",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) static bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "enableLayoutAnimationsOnIOS",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) static bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "enableLongTaskAPI",
          "    REACT_SYNC_METHOD(enableLongTaskAPI) bool enableLongTaskAPI() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLongTaskAPI) static bool enableLongTaskAPI() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "enableNewBackgroundAndBorderDrawables",
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) static bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "enablePreciseSchedulingForPremountItemsOnAndroid",
          "    REACT_SYNC_METHOD(enablePreciseSchedulingForPremountItemsOnAndroid) bool enablePreciseSchedulingForPremountItemsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePreciseSchedulingForPremountItemsOnAndroid) static bool enablePreciseSchedulingForPremountItemsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "enablePropsUpdateReconciliationAndroid",
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) static bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "enableReportEventPaintTime",
          "    REACT_SYNC_METHOD(enableReportEventPaintTime) bool enableReportEventPaintTime() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableReportEventPaintTime) static bool enableReportEventPaintTime() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "enableSynchronousStateUpdates",
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) static bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "enableUIConsistency",
          "    REACT_SYNC_METHOD(enableUIConsistency) bool enableUIConsistency() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableUIConsistency) static bool enableUIConsistency() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "enableViewRecycling",
          "    REACT_SYNC_METHOD(enableViewRecycling) bool enableViewRecycling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecycling) static bool enableViewRecycling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "excludeYogaFromRawProps",
          "    REACT_SYNC_METHOD(excludeYogaFromRawProps) bool excludeYogaFromRawProps() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(excludeYogaFromRawProps) static bool excludeYogaFromRawProps() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          29,
          "fixMappingOfEventPrioritiesBetweenFabricAndReact",
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) static bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          30,
          "fixMountingCoordinatorReportedPendingTransactionsOnAndroid",
          "    REACT_SYNC_METHOD(fixMountingCoordinatorReportedPendingTransactionsOnAndroid) bool fixMountingCoordinatorReportedPendingTransactionsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fixMountingCoordinatorReportedPendingTransactionsOnAndroid) static bool fixMountingCoordinatorReportedPendingTransactionsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          31,
          "fuseboxEnabledDebug",
          "    REACT_SYNC_METHOD(fuseboxEnabledDebug) bool fuseboxEnabledDebug() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledDebug) static bool fuseboxEnabledDebug() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "fuseboxEnabledRelease",
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) static bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "initEagerTurboModulesOnNativeModulesQueueAndroid",
          "    REACT_SYNC_METHOD(initEagerTurboModulesOnNativeModulesQueueAndroid) bool initEagerTurboModulesOnNativeModulesQueueAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(initEagerTurboModulesOnNativeModulesQueueAndroid) static bool initEagerTurboModulesOnNativeModulesQueueAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "lazyAnimationCallbacks",
          "    REACT_SYNC_METHOD(lazyAnimationCallbacks) bool lazyAnimationCallbacks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(lazyAnimationCallbacks) static bool lazyAnimationCallbacks() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "loadVectorDrawablesOnImages",
          "    REACT_SYNC_METHOD(loadVectorDrawablesOnImages) bool loadVectorDrawablesOnImages() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(loadVectorDrawablesOnImages) static bool loadVectorDrawablesOnImages() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "setAndroidLayoutDirection",
          "    REACT_SYNC_METHOD(setAndroidLayoutDirection) bool setAndroidLayoutDirection() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(setAndroidLayoutDirection) static bool setAndroidLayoutDirection() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "traceTurboModulePromiseRejectionsOnAndroid",
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) static bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "useAlwaysAvailableJSErrorHandling",
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) static bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          39,
          "useFabricInterop",
          "    REACT_SYNC_METHOD(useFabricInterop) bool useFabricInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useFabricInterop) static bool useFabricInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          40,
          "useImmediateExecutorInAndroidBridgeless",
          "    REACT_SYNC_METHOD(useImmediateExecutorInAndroidBridgeless) bool useImmediateExecutorInAndroidBridgeless() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useImmediateExecutorInAndroidBridgeless) static bool useImmediateExecutorInAndroidBridgeless() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          41,
          "useNativeViewConfigsInBridgelessMode",
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) static bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          42,
          "useOptimisedViewPreallocationOnAndroid",
          "    REACT_SYNC_METHOD(useOptimisedViewPreallocationOnAndroid) bool useOptimisedViewPreallocationOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimisedViewPreallocationOnAndroid) static bool useOptimisedViewPreallocationOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          43,
          "useOptimizedEventBatchingOnAndroid",
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) static bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          44,
          "useRuntimeShadowNodeReferenceUpdate",
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdate) bool useRuntimeShadowNodeReferenceUpdate() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdate) static bool useRuntimeShadowNodeReferenceUpdate() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          45,
          "useTurboModuleInterop",
          "    REACT_SYNC_METHOD(useTurboModuleInterop) bool useTurboModuleInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModuleInterop) static bool useTurboModuleInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          46,
          "useTurboModules",
          "    REACT_SYNC_METHOD(useTurboModules) bool useTurboModules() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModules) static bool useTurboModules() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
