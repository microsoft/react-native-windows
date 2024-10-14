
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
      SyncMethod<bool() noexcept>{1, L"allowRecursiveCommitsWithSynchronousMountOnAndroid"},
      SyncMethod<bool() noexcept>{2, L"batchRenderingUpdatesInEventLoop"},
      SyncMethod<bool() noexcept>{3, L"completeReactInstanceCreationOnBgThreadOnAndroid"},
      SyncMethod<bool() noexcept>{4, L"destroyFabricSurfacesInReactInstanceManager"},
      SyncMethod<bool() noexcept>{5, L"enableAlignItemsBaselineOnFabricIOS"},
      SyncMethod<bool() noexcept>{6, L"enableAndroidLineHeightCentering"},
      SyncMethod<bool() noexcept>{7, L"enableAndroidMixBlendModeProp"},
      SyncMethod<bool() noexcept>{8, L"enableBackgroundStyleApplicator"},
      SyncMethod<bool() noexcept>{9, L"enableCleanTextInputYogaNode"},
      SyncMethod<bool() noexcept>{10, L"enableDeletionOfUnmountedViews"},
      SyncMethod<bool() noexcept>{11, L"enableEagerRootViewAttachment"},
      SyncMethod<bool() noexcept>{12, L"enableEventEmitterRetentionDuringGesturesOnAndroid"},
      SyncMethod<bool() noexcept>{13, L"enableFabricLogs"},
      SyncMethod<bool() noexcept>{14, L"enableFabricRendererExclusively"},
      SyncMethod<bool() noexcept>{15, L"enableGranularShadowTreeStateReconciliation"},
      SyncMethod<bool() noexcept>{16, L"enableIOSViewClipToPaddingBox"},
      SyncMethod<bool() noexcept>{17, L"enableLayoutAnimationsOnIOS"},
      SyncMethod<bool() noexcept>{18, L"enableLongTaskAPI"},
      SyncMethod<bool() noexcept>{19, L"enableMicrotasks"},
      SyncMethod<bool() noexcept>{20, L"enablePreciseSchedulingForPremountItemsOnAndroid"},
      SyncMethod<bool() noexcept>{21, L"enablePropsUpdateReconciliationAndroid"},
      SyncMethod<bool() noexcept>{22, L"enableReportEventPaintTime"},
      SyncMethod<bool() noexcept>{23, L"enableSynchronousStateUpdates"},
      SyncMethod<bool() noexcept>{24, L"enableTextPreallocationOptimisation"},
      SyncMethod<bool() noexcept>{25, L"enableUIConsistency"},
      SyncMethod<bool() noexcept>{26, L"enableViewRecycling"},
      SyncMethod<bool() noexcept>{27, L"excludeYogaFromRawProps"},
      SyncMethod<bool() noexcept>{28, L"fetchImagesInViewPreallocation"},
      SyncMethod<bool() noexcept>{29, L"fixMappingOfEventPrioritiesBetweenFabricAndReact"},
      SyncMethod<bool() noexcept>{30, L"fixMountingCoordinatorReportedPendingTransactionsOnAndroid"},
      SyncMethod<bool() noexcept>{31, L"forceBatchingMountItemsOnAndroid"},
      SyncMethod<bool() noexcept>{32, L"fuseboxEnabledDebug"},
      SyncMethod<bool() noexcept>{33, L"fuseboxEnabledRelease"},
      SyncMethod<bool() noexcept>{34, L"initEagerTurboModulesOnNativeModulesQueueAndroid"},
      SyncMethod<bool() noexcept>{35, L"lazyAnimationCallbacks"},
      SyncMethod<bool() noexcept>{36, L"loadVectorDrawablesOnImages"},
      SyncMethod<bool() noexcept>{37, L"removeNestedCallsToDispatchMountItemsOnAndroid"},
      SyncMethod<bool() noexcept>{38, L"setAndroidLayoutDirection"},
      SyncMethod<bool() noexcept>{39, L"traceTurboModulePromiseRejectionsOnAndroid"},
      SyncMethod<bool() noexcept>{40, L"useFabricInterop"},
      SyncMethod<bool() noexcept>{41, L"useImmediateExecutorInAndroidBridgeless"},
      SyncMethod<bool() noexcept>{42, L"useModernRuntimeScheduler"},
      SyncMethod<bool() noexcept>{43, L"useNativeViewConfigsInBridgelessMode"},
      SyncMethod<bool() noexcept>{44, L"useNewReactImageViewBackgroundDrawing"},
      SyncMethod<bool() noexcept>{45, L"useOptimisedViewPreallocationOnAndroid"},
      SyncMethod<bool() noexcept>{46, L"useOptimizedEventBatchingOnAndroid"},
      SyncMethod<bool() noexcept>{47, L"useRuntimeShadowNodeReferenceUpdate"},
      SyncMethod<bool() noexcept>{48, L"useRuntimeShadowNodeReferenceUpdateOnLayout"},
      SyncMethod<bool() noexcept>{49, L"useStateAlignmentMechanism"},
      SyncMethod<bool() noexcept>{50, L"useTurboModuleInterop"},
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
          "allowRecursiveCommitsWithSynchronousMountOnAndroid",
          "    REACT_SYNC_METHOD(allowRecursiveCommitsWithSynchronousMountOnAndroid) bool allowRecursiveCommitsWithSynchronousMountOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(allowRecursiveCommitsWithSynchronousMountOnAndroid) static bool allowRecursiveCommitsWithSynchronousMountOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "batchRenderingUpdatesInEventLoop",
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(batchRenderingUpdatesInEventLoop) static bool batchRenderingUpdatesInEventLoop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "completeReactInstanceCreationOnBgThreadOnAndroid",
          "    REACT_SYNC_METHOD(completeReactInstanceCreationOnBgThreadOnAndroid) bool completeReactInstanceCreationOnBgThreadOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(completeReactInstanceCreationOnBgThreadOnAndroid) static bool completeReactInstanceCreationOnBgThreadOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "destroyFabricSurfacesInReactInstanceManager",
          "    REACT_SYNC_METHOD(destroyFabricSurfacesInReactInstanceManager) bool destroyFabricSurfacesInReactInstanceManager() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(destroyFabricSurfacesInReactInstanceManager) static bool destroyFabricSurfacesInReactInstanceManager() noexcept { /* implementation */ }\n");
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
          "enableAndroidMixBlendModeProp",
          "    REACT_SYNC_METHOD(enableAndroidMixBlendModeProp) bool enableAndroidMixBlendModeProp() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAndroidMixBlendModeProp) static bool enableAndroidMixBlendModeProp() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableBackgroundStyleApplicator",
          "    REACT_SYNC_METHOD(enableBackgroundStyleApplicator) bool enableBackgroundStyleApplicator() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBackgroundStyleApplicator) static bool enableBackgroundStyleApplicator() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableCleanTextInputYogaNode",
          "    REACT_SYNC_METHOD(enableCleanTextInputYogaNode) bool enableCleanTextInputYogaNode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCleanTextInputYogaNode) static bool enableCleanTextInputYogaNode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "enableDeletionOfUnmountedViews",
          "    REACT_SYNC_METHOD(enableDeletionOfUnmountedViews) bool enableDeletionOfUnmountedViews() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDeletionOfUnmountedViews) static bool enableDeletionOfUnmountedViews() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "enableEagerRootViewAttachment",
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) static bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "enableEventEmitterRetentionDuringGesturesOnAndroid",
          "    REACT_SYNC_METHOD(enableEventEmitterRetentionDuringGesturesOnAndroid) bool enableEventEmitterRetentionDuringGesturesOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEventEmitterRetentionDuringGesturesOnAndroid) static bool enableEventEmitterRetentionDuringGesturesOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "enableFabricLogs",
          "    REACT_SYNC_METHOD(enableFabricLogs) bool enableFabricLogs() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricLogs) static bool enableFabricLogs() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "enableFabricRendererExclusively",
          "    REACT_SYNC_METHOD(enableFabricRendererExclusively) bool enableFabricRendererExclusively() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricRendererExclusively) static bool enableFabricRendererExclusively() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "enableGranularShadowTreeStateReconciliation",
          "    REACT_SYNC_METHOD(enableGranularShadowTreeStateReconciliation) bool enableGranularShadowTreeStateReconciliation() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableGranularShadowTreeStateReconciliation) static bool enableGranularShadowTreeStateReconciliation() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "enableIOSViewClipToPaddingBox",
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) static bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "enableLayoutAnimationsOnIOS",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) static bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "enableLongTaskAPI",
          "    REACT_SYNC_METHOD(enableLongTaskAPI) bool enableLongTaskAPI() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLongTaskAPI) static bool enableLongTaskAPI() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "enableMicrotasks",
          "    REACT_SYNC_METHOD(enableMicrotasks) bool enableMicrotasks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMicrotasks) static bool enableMicrotasks() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "enablePreciseSchedulingForPremountItemsOnAndroid",
          "    REACT_SYNC_METHOD(enablePreciseSchedulingForPremountItemsOnAndroid) bool enablePreciseSchedulingForPremountItemsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePreciseSchedulingForPremountItemsOnAndroid) static bool enablePreciseSchedulingForPremountItemsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "enablePropsUpdateReconciliationAndroid",
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) static bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "enableReportEventPaintTime",
          "    REACT_SYNC_METHOD(enableReportEventPaintTime) bool enableReportEventPaintTime() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableReportEventPaintTime) static bool enableReportEventPaintTime() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "enableSynchronousStateUpdates",
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) static bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "enableTextPreallocationOptimisation",
          "    REACT_SYNC_METHOD(enableTextPreallocationOptimisation) bool enableTextPreallocationOptimisation() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableTextPreallocationOptimisation) static bool enableTextPreallocationOptimisation() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "enableUIConsistency",
          "    REACT_SYNC_METHOD(enableUIConsistency) bool enableUIConsistency() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableUIConsistency) static bool enableUIConsistency() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "enableViewRecycling",
          "    REACT_SYNC_METHOD(enableViewRecycling) bool enableViewRecycling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecycling) static bool enableViewRecycling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "excludeYogaFromRawProps",
          "    REACT_SYNC_METHOD(excludeYogaFromRawProps) bool excludeYogaFromRawProps() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(excludeYogaFromRawProps) static bool excludeYogaFromRawProps() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "fetchImagesInViewPreallocation",
          "    REACT_SYNC_METHOD(fetchImagesInViewPreallocation) bool fetchImagesInViewPreallocation() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fetchImagesInViewPreallocation) static bool fetchImagesInViewPreallocation() noexcept { /* implementation */ }\n");
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
          "forceBatchingMountItemsOnAndroid",
          "    REACT_SYNC_METHOD(forceBatchingMountItemsOnAndroid) bool forceBatchingMountItemsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(forceBatchingMountItemsOnAndroid) static bool forceBatchingMountItemsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "fuseboxEnabledDebug",
          "    REACT_SYNC_METHOD(fuseboxEnabledDebug) bool fuseboxEnabledDebug() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledDebug) static bool fuseboxEnabledDebug() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "fuseboxEnabledRelease",
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) static bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "initEagerTurboModulesOnNativeModulesQueueAndroid",
          "    REACT_SYNC_METHOD(initEagerTurboModulesOnNativeModulesQueueAndroid) bool initEagerTurboModulesOnNativeModulesQueueAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(initEagerTurboModulesOnNativeModulesQueueAndroid) static bool initEagerTurboModulesOnNativeModulesQueueAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "lazyAnimationCallbacks",
          "    REACT_SYNC_METHOD(lazyAnimationCallbacks) bool lazyAnimationCallbacks() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(lazyAnimationCallbacks) static bool lazyAnimationCallbacks() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "loadVectorDrawablesOnImages",
          "    REACT_SYNC_METHOD(loadVectorDrawablesOnImages) bool loadVectorDrawablesOnImages() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(loadVectorDrawablesOnImages) static bool loadVectorDrawablesOnImages() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "removeNestedCallsToDispatchMountItemsOnAndroid",
          "    REACT_SYNC_METHOD(removeNestedCallsToDispatchMountItemsOnAndroid) bool removeNestedCallsToDispatchMountItemsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(removeNestedCallsToDispatchMountItemsOnAndroid) static bool removeNestedCallsToDispatchMountItemsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "setAndroidLayoutDirection",
          "    REACT_SYNC_METHOD(setAndroidLayoutDirection) bool setAndroidLayoutDirection() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(setAndroidLayoutDirection) static bool setAndroidLayoutDirection() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          39,
          "traceTurboModulePromiseRejectionsOnAndroid",
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) static bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          40,
          "useFabricInterop",
          "    REACT_SYNC_METHOD(useFabricInterop) bool useFabricInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useFabricInterop) static bool useFabricInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          41,
          "useImmediateExecutorInAndroidBridgeless",
          "    REACT_SYNC_METHOD(useImmediateExecutorInAndroidBridgeless) bool useImmediateExecutorInAndroidBridgeless() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useImmediateExecutorInAndroidBridgeless) static bool useImmediateExecutorInAndroidBridgeless() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          42,
          "useModernRuntimeScheduler",
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useModernRuntimeScheduler) static bool useModernRuntimeScheduler() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          43,
          "useNativeViewConfigsInBridgelessMode",
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) static bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          44,
          "useNewReactImageViewBackgroundDrawing",
          "    REACT_SYNC_METHOD(useNewReactImageViewBackgroundDrawing) bool useNewReactImageViewBackgroundDrawing() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNewReactImageViewBackgroundDrawing) static bool useNewReactImageViewBackgroundDrawing() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          45,
          "useOptimisedViewPreallocationOnAndroid",
          "    REACT_SYNC_METHOD(useOptimisedViewPreallocationOnAndroid) bool useOptimisedViewPreallocationOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimisedViewPreallocationOnAndroid) static bool useOptimisedViewPreallocationOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          46,
          "useOptimizedEventBatchingOnAndroid",
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) static bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          47,
          "useRuntimeShadowNodeReferenceUpdate",
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdate) bool useRuntimeShadowNodeReferenceUpdate() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdate) static bool useRuntimeShadowNodeReferenceUpdate() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          48,
          "useRuntimeShadowNodeReferenceUpdateOnLayout",
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdateOnLayout) bool useRuntimeShadowNodeReferenceUpdateOnLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useRuntimeShadowNodeReferenceUpdateOnLayout) static bool useRuntimeShadowNodeReferenceUpdateOnLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          49,
          "useStateAlignmentMechanism",
          "    REACT_SYNC_METHOD(useStateAlignmentMechanism) bool useStateAlignmentMechanism() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useStateAlignmentMechanism) static bool useStateAlignmentMechanism() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          50,
          "useTurboModuleInterop",
          "    REACT_SYNC_METHOD(useTurboModuleInterop) bool useTurboModuleInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModuleInterop) static bool useTurboModuleInterop() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
