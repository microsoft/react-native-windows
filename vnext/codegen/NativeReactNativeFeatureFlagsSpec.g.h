
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
      SyncMethod<bool() noexcept>{1, L"commonTestFlagWithoutNativeImplementation"},
      SyncMethod<bool() noexcept>{2, L"animatedShouldSignalBatch"},
      SyncMethod<bool() noexcept>{3, L"cxxNativeAnimatedEnabled"},
      SyncMethod<bool() noexcept>{4, L"cxxNativeAnimatedRemoveJsSync"},
      SyncMethod<bool() noexcept>{5, L"disableMainQueueSyncDispatchIOS"},
      SyncMethod<bool() noexcept>{6, L"disableMountItemReorderingAndroid"},
      SyncMethod<bool() noexcept>{7, L"disableTextLayoutManagerCacheAndroid"},
      SyncMethod<bool() noexcept>{8, L"enableAccessibilityOrder"},
      SyncMethod<bool() noexcept>{9, L"enableAccumulatedUpdatesInRawPropsAndroid"},
      SyncMethod<bool() noexcept>{10, L"enableAndroidTextMeasurementOptimizations"},
      SyncMethod<bool() noexcept>{11, L"enableBridgelessArchitecture"},
      SyncMethod<bool() noexcept>{12, L"enableCppPropsIteratorSetter"},
      SyncMethod<bool() noexcept>{13, L"enableCustomFocusSearchOnClippedElementsAndroid"},
      SyncMethod<bool() noexcept>{14, L"enableDestroyShadowTreeRevisionAsync"},
      SyncMethod<bool() noexcept>{15, L"enableDoubleMeasurementFixAndroid"},
      SyncMethod<bool() noexcept>{16, L"enableEagerRootViewAttachment"},
      SyncMethod<bool() noexcept>{17, L"enableFabricLogs"},
      SyncMethod<bool() noexcept>{18, L"enableFabricRenderer"},
      SyncMethod<bool() noexcept>{19, L"enableFixForParentTagDuringReparenting"},
      SyncMethod<bool() noexcept>{20, L"enableFontScaleChangesUpdatingLayout"},
      SyncMethod<bool() noexcept>{21, L"enableIOSTextBaselineOffsetPerLine"},
      SyncMethod<bool() noexcept>{22, L"enableIOSViewClipToPaddingBox"},
      SyncMethod<bool() noexcept>{23, L"enableInteropViewManagerClassLookUpOptimizationIOS"},
      SyncMethod<bool() noexcept>{24, L"enableLayoutAnimationsOnAndroid"},
      SyncMethod<bool() noexcept>{25, L"enableLayoutAnimationsOnIOS"},
      SyncMethod<bool() noexcept>{26, L"enableMainQueueCoordinatorOnIOS"},
      SyncMethod<bool() noexcept>{27, L"enableMainQueueModulesOnIOS"},
      SyncMethod<bool() noexcept>{28, L"enableModuleArgumentNSNullConversionIOS"},
      SyncMethod<bool() noexcept>{29, L"enableNativeCSSParsing"},
      SyncMethod<bool() noexcept>{30, L"enableNetworkEventReporting"},
      SyncMethod<bool() noexcept>{31, L"enableNewBackgroundAndBorderDrawables"},
      SyncMethod<bool() noexcept>{32, L"enablePreparedTextLayout"},
      SyncMethod<bool() noexcept>{33, L"enablePropsUpdateReconciliationAndroid"},
      SyncMethod<bool() noexcept>{34, L"enableResourceTimingAPI"},
      SyncMethod<bool() noexcept>{35, L"enableSynchronousStateUpdates"},
      SyncMethod<bool() noexcept>{36, L"enableViewCulling"},
      SyncMethod<bool() noexcept>{37, L"enableViewRecycling"},
      SyncMethod<bool() noexcept>{38, L"enableViewRecyclingForText"},
      SyncMethod<bool() noexcept>{39, L"enableViewRecyclingForView"},
      SyncMethod<bool() noexcept>{40, L"enableVirtualViewDebugFeatures"},
      SyncMethod<bool() noexcept>{41, L"enableVirtualViewRenderState"},
      SyncMethod<bool() noexcept>{42, L"enableVirtualViewWindowFocusDetection"},
      SyncMethod<bool() noexcept>{43, L"fixMappingOfEventPrioritiesBetweenFabricAndReact"},
      SyncMethod<bool() noexcept>{44, L"fuseboxEnabledRelease"},
      SyncMethod<bool() noexcept>{45, L"fuseboxNetworkInspectionEnabled"},
      SyncMethod<bool() noexcept>{46, L"hideOffscreenVirtualViewsOnIOS"},
      SyncMethod<double() noexcept>{47, L"preparedTextCacheSize"},
      SyncMethod<bool() noexcept>{48, L"preventShadowTreeCommitExhaustion"},
      SyncMethod<bool() noexcept>{49, L"traceTurboModulePromiseRejectionsOnAndroid"},
      SyncMethod<bool() noexcept>{50, L"updateRuntimeShadowNodeReferencesOnCommit"},
      SyncMethod<bool() noexcept>{51, L"useAlwaysAvailableJSErrorHandling"},
      SyncMethod<bool() noexcept>{52, L"useFabricInterop"},
      SyncMethod<bool() noexcept>{53, L"useNativeEqualsInNativeReadableArrayAndroid"},
      SyncMethod<bool() noexcept>{54, L"useNativeTransformHelperAndroid"},
      SyncMethod<bool() noexcept>{55, L"useNativeViewConfigsInBridgelessMode"},
      SyncMethod<bool() noexcept>{56, L"useOptimizedEventBatchingOnAndroid"},
      SyncMethod<bool() noexcept>{57, L"useRawPropsJsiValue"},
      SyncMethod<bool() noexcept>{58, L"useShadowNodeStateOnClone"},
      SyncMethod<bool() noexcept>{59, L"useTurboModuleInterop"},
      SyncMethod<bool() noexcept>{60, L"useTurboModules"},
      SyncMethod<double() noexcept>{61, L"virtualViewPrerenderRatio"},
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
          "animatedShouldSignalBatch",
          "    REACT_SYNC_METHOD(animatedShouldSignalBatch) bool animatedShouldSignalBatch() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(animatedShouldSignalBatch) static bool animatedShouldSignalBatch() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "cxxNativeAnimatedEnabled",
          "    REACT_SYNC_METHOD(cxxNativeAnimatedEnabled) bool cxxNativeAnimatedEnabled() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(cxxNativeAnimatedEnabled) static bool cxxNativeAnimatedEnabled() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "cxxNativeAnimatedRemoveJsSync",
          "    REACT_SYNC_METHOD(cxxNativeAnimatedRemoveJsSync) bool cxxNativeAnimatedRemoveJsSync() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(cxxNativeAnimatedRemoveJsSync) static bool cxxNativeAnimatedRemoveJsSync() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "disableMainQueueSyncDispatchIOS",
          "    REACT_SYNC_METHOD(disableMainQueueSyncDispatchIOS) bool disableMainQueueSyncDispatchIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(disableMainQueueSyncDispatchIOS) static bool disableMainQueueSyncDispatchIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "disableMountItemReorderingAndroid",
          "    REACT_SYNC_METHOD(disableMountItemReorderingAndroid) bool disableMountItemReorderingAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(disableMountItemReorderingAndroid) static bool disableMountItemReorderingAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "disableTextLayoutManagerCacheAndroid",
          "    REACT_SYNC_METHOD(disableTextLayoutManagerCacheAndroid) bool disableTextLayoutManagerCacheAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(disableTextLayoutManagerCacheAndroid) static bool disableTextLayoutManagerCacheAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableAccessibilityOrder",
          "    REACT_SYNC_METHOD(enableAccessibilityOrder) bool enableAccessibilityOrder() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAccessibilityOrder) static bool enableAccessibilityOrder() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableAccumulatedUpdatesInRawPropsAndroid",
          "    REACT_SYNC_METHOD(enableAccumulatedUpdatesInRawPropsAndroid) bool enableAccumulatedUpdatesInRawPropsAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAccumulatedUpdatesInRawPropsAndroid) static bool enableAccumulatedUpdatesInRawPropsAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "enableAndroidTextMeasurementOptimizations",
          "    REACT_SYNC_METHOD(enableAndroidTextMeasurementOptimizations) bool enableAndroidTextMeasurementOptimizations() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAndroidTextMeasurementOptimizations) static bool enableAndroidTextMeasurementOptimizations() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "enableBridgelessArchitecture",
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) static bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "enableCppPropsIteratorSetter",
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) static bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "enableCustomFocusSearchOnClippedElementsAndroid",
          "    REACT_SYNC_METHOD(enableCustomFocusSearchOnClippedElementsAndroid) bool enableCustomFocusSearchOnClippedElementsAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCustomFocusSearchOnClippedElementsAndroid) static bool enableCustomFocusSearchOnClippedElementsAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "enableDestroyShadowTreeRevisionAsync",
          "    REACT_SYNC_METHOD(enableDestroyShadowTreeRevisionAsync) bool enableDestroyShadowTreeRevisionAsync() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDestroyShadowTreeRevisionAsync) static bool enableDestroyShadowTreeRevisionAsync() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "enableDoubleMeasurementFixAndroid",
          "    REACT_SYNC_METHOD(enableDoubleMeasurementFixAndroid) bool enableDoubleMeasurementFixAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDoubleMeasurementFixAndroid) static bool enableDoubleMeasurementFixAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "enableEagerRootViewAttachment",
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) static bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "enableFabricLogs",
          "    REACT_SYNC_METHOD(enableFabricLogs) bool enableFabricLogs() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricLogs) static bool enableFabricLogs() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "enableFabricRenderer",
          "    REACT_SYNC_METHOD(enableFabricRenderer) bool enableFabricRenderer() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricRenderer) static bool enableFabricRenderer() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "enableFixForParentTagDuringReparenting",
          "    REACT_SYNC_METHOD(enableFixForParentTagDuringReparenting) bool enableFixForParentTagDuringReparenting() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFixForParentTagDuringReparenting) static bool enableFixForParentTagDuringReparenting() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "enableFontScaleChangesUpdatingLayout",
          "    REACT_SYNC_METHOD(enableFontScaleChangesUpdatingLayout) bool enableFontScaleChangesUpdatingLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFontScaleChangesUpdatingLayout) static bool enableFontScaleChangesUpdatingLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "enableIOSTextBaselineOffsetPerLine",
          "    REACT_SYNC_METHOD(enableIOSTextBaselineOffsetPerLine) bool enableIOSTextBaselineOffsetPerLine() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIOSTextBaselineOffsetPerLine) static bool enableIOSTextBaselineOffsetPerLine() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "enableIOSViewClipToPaddingBox",
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) static bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "enableInteropViewManagerClassLookUpOptimizationIOS",
          "    REACT_SYNC_METHOD(enableInteropViewManagerClassLookUpOptimizationIOS) bool enableInteropViewManagerClassLookUpOptimizationIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableInteropViewManagerClassLookUpOptimizationIOS) static bool enableInteropViewManagerClassLookUpOptimizationIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "enableLayoutAnimationsOnAndroid",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) static bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "enableLayoutAnimationsOnIOS",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) static bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "enableMainQueueCoordinatorOnIOS",
          "    REACT_SYNC_METHOD(enableMainQueueCoordinatorOnIOS) bool enableMainQueueCoordinatorOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMainQueueCoordinatorOnIOS) static bool enableMainQueueCoordinatorOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "enableMainQueueModulesOnIOS",
          "    REACT_SYNC_METHOD(enableMainQueueModulesOnIOS) bool enableMainQueueModulesOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMainQueueModulesOnIOS) static bool enableMainQueueModulesOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "enableModuleArgumentNSNullConversionIOS",
          "    REACT_SYNC_METHOD(enableModuleArgumentNSNullConversionIOS) bool enableModuleArgumentNSNullConversionIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableModuleArgumentNSNullConversionIOS) static bool enableModuleArgumentNSNullConversionIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          29,
          "enableNativeCSSParsing",
          "    REACT_SYNC_METHOD(enableNativeCSSParsing) bool enableNativeCSSParsing() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNativeCSSParsing) static bool enableNativeCSSParsing() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          30,
          "enableNetworkEventReporting",
          "    REACT_SYNC_METHOD(enableNetworkEventReporting) bool enableNetworkEventReporting() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNetworkEventReporting) static bool enableNetworkEventReporting() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          31,
          "enableNewBackgroundAndBorderDrawables",
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) static bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "enablePreparedTextLayout",
          "    REACT_SYNC_METHOD(enablePreparedTextLayout) bool enablePreparedTextLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePreparedTextLayout) static bool enablePreparedTextLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "enablePropsUpdateReconciliationAndroid",
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) static bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "enableResourceTimingAPI",
          "    REACT_SYNC_METHOD(enableResourceTimingAPI) bool enableResourceTimingAPI() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableResourceTimingAPI) static bool enableResourceTimingAPI() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "enableSynchronousStateUpdates",
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) static bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "enableViewCulling",
          "    REACT_SYNC_METHOD(enableViewCulling) bool enableViewCulling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewCulling) static bool enableViewCulling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "enableViewRecycling",
          "    REACT_SYNC_METHOD(enableViewRecycling) bool enableViewRecycling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecycling) static bool enableViewRecycling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "enableViewRecyclingForText",
          "    REACT_SYNC_METHOD(enableViewRecyclingForText) bool enableViewRecyclingForText() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecyclingForText) static bool enableViewRecyclingForText() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          39,
          "enableViewRecyclingForView",
          "    REACT_SYNC_METHOD(enableViewRecyclingForView) bool enableViewRecyclingForView() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecyclingForView) static bool enableViewRecyclingForView() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          40,
          "enableVirtualViewDebugFeatures",
          "    REACT_SYNC_METHOD(enableVirtualViewDebugFeatures) bool enableVirtualViewDebugFeatures() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableVirtualViewDebugFeatures) static bool enableVirtualViewDebugFeatures() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          41,
          "enableVirtualViewRenderState",
          "    REACT_SYNC_METHOD(enableVirtualViewRenderState) bool enableVirtualViewRenderState() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableVirtualViewRenderState) static bool enableVirtualViewRenderState() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          42,
          "enableVirtualViewWindowFocusDetection",
          "    REACT_SYNC_METHOD(enableVirtualViewWindowFocusDetection) bool enableVirtualViewWindowFocusDetection() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableVirtualViewWindowFocusDetection) static bool enableVirtualViewWindowFocusDetection() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          43,
          "fixMappingOfEventPrioritiesBetweenFabricAndReact",
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) static bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          44,
          "fuseboxEnabledRelease",
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) static bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          45,
          "fuseboxNetworkInspectionEnabled",
          "    REACT_SYNC_METHOD(fuseboxNetworkInspectionEnabled) bool fuseboxNetworkInspectionEnabled() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxNetworkInspectionEnabled) static bool fuseboxNetworkInspectionEnabled() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          46,
          "hideOffscreenVirtualViewsOnIOS",
          "    REACT_SYNC_METHOD(hideOffscreenVirtualViewsOnIOS) bool hideOffscreenVirtualViewsOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(hideOffscreenVirtualViewsOnIOS) static bool hideOffscreenVirtualViewsOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          47,
          "preparedTextCacheSize",
          "    REACT_SYNC_METHOD(preparedTextCacheSize) double preparedTextCacheSize() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(preparedTextCacheSize) static double preparedTextCacheSize() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          48,
          "preventShadowTreeCommitExhaustion",
          "    REACT_SYNC_METHOD(preventShadowTreeCommitExhaustion) bool preventShadowTreeCommitExhaustion() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(preventShadowTreeCommitExhaustion) static bool preventShadowTreeCommitExhaustion() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          49,
          "traceTurboModulePromiseRejectionsOnAndroid",
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) static bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          50,
          "updateRuntimeShadowNodeReferencesOnCommit",
          "    REACT_SYNC_METHOD(updateRuntimeShadowNodeReferencesOnCommit) bool updateRuntimeShadowNodeReferencesOnCommit() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(updateRuntimeShadowNodeReferencesOnCommit) static bool updateRuntimeShadowNodeReferencesOnCommit() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          51,
          "useAlwaysAvailableJSErrorHandling",
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) static bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          52,
          "useFabricInterop",
          "    REACT_SYNC_METHOD(useFabricInterop) bool useFabricInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useFabricInterop) static bool useFabricInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          53,
          "useNativeEqualsInNativeReadableArrayAndroid",
          "    REACT_SYNC_METHOD(useNativeEqualsInNativeReadableArrayAndroid) bool useNativeEqualsInNativeReadableArrayAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeEqualsInNativeReadableArrayAndroid) static bool useNativeEqualsInNativeReadableArrayAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          54,
          "useNativeTransformHelperAndroid",
          "    REACT_SYNC_METHOD(useNativeTransformHelperAndroid) bool useNativeTransformHelperAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeTransformHelperAndroid) static bool useNativeTransformHelperAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          55,
          "useNativeViewConfigsInBridgelessMode",
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) static bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          56,
          "useOptimizedEventBatchingOnAndroid",
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) static bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          57,
          "useRawPropsJsiValue",
          "    REACT_SYNC_METHOD(useRawPropsJsiValue) bool useRawPropsJsiValue() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useRawPropsJsiValue) static bool useRawPropsJsiValue() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          58,
          "useShadowNodeStateOnClone",
          "    REACT_SYNC_METHOD(useShadowNodeStateOnClone) bool useShadowNodeStateOnClone() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useShadowNodeStateOnClone) static bool useShadowNodeStateOnClone() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          59,
          "useTurboModuleInterop",
          "    REACT_SYNC_METHOD(useTurboModuleInterop) bool useTurboModuleInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModuleInterop) static bool useTurboModuleInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          60,
          "useTurboModules",
          "    REACT_SYNC_METHOD(useTurboModules) bool useTurboModules() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModules) static bool useTurboModules() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          61,
          "virtualViewPrerenderRatio",
          "    REACT_SYNC_METHOD(virtualViewPrerenderRatio) double virtualViewPrerenderRatio() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(virtualViewPrerenderRatio) static double virtualViewPrerenderRatio() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
