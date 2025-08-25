
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
      SyncMethod<bool() noexcept>{3, L"avoidCeilingAvailableAndroidTextWidth"},
      SyncMethod<bool() noexcept>{4, L"cxxNativeAnimatedEnabled"},
      SyncMethod<bool() noexcept>{5, L"disableMainQueueSyncDispatchIOS"},
      SyncMethod<bool() noexcept>{6, L"disableMountItemReorderingAndroid"},
      SyncMethod<bool() noexcept>{7, L"enableAccessibilityOrder"},
      SyncMethod<bool() noexcept>{8, L"enableAccumulatedUpdatesInRawPropsAndroid"},
      SyncMethod<bool() noexcept>{9, L"enableBridgelessArchitecture"},
      SyncMethod<bool() noexcept>{10, L"enableCppPropsIteratorSetter"},
      SyncMethod<bool() noexcept>{11, L"enableCustomFocusSearchOnClippedElementsAndroid"},
      SyncMethod<bool() noexcept>{12, L"enableDestroyShadowTreeRevisionAsync"},
      SyncMethod<bool() noexcept>{13, L"enableDoubleMeasurementFixAndroid"},
      SyncMethod<bool() noexcept>{14, L"enableEagerRootViewAttachment"},
      SyncMethod<bool() noexcept>{15, L"enableFabricLogs"},
      SyncMethod<bool() noexcept>{16, L"enableFabricRenderer"},
      SyncMethod<bool() noexcept>{17, L"enableFixForParentTagDuringReparenting"},
      SyncMethod<bool() noexcept>{18, L"enableFontScaleChangesUpdatingLayout"},
      SyncMethod<bool() noexcept>{19, L"enableIOSViewClipToPaddingBox"},
      SyncMethod<bool() noexcept>{20, L"enableIntersectionObserverEventLoopIntegration"},
      SyncMethod<bool() noexcept>{21, L"enableLayoutAnimationsOnAndroid"},
      SyncMethod<bool() noexcept>{22, L"enableLayoutAnimationsOnIOS"},
      SyncMethod<bool() noexcept>{23, L"enableMainQueueModulesOnIOS"},
      SyncMethod<bool() noexcept>{24, L"enableNativeCSSParsing"},
      SyncMethod<bool() noexcept>{25, L"enableNetworkEventReporting"},
      SyncMethod<bool() noexcept>{26, L"enableNewBackgroundAndBorderDrawables"},
      SyncMethod<bool() noexcept>{27, L"enablePreparedTextLayout"},
      SyncMethod<bool() noexcept>{28, L"enablePropsUpdateReconciliationAndroid"},
      SyncMethod<bool() noexcept>{29, L"enableResourceTimingAPI"},
      SyncMethod<bool() noexcept>{30, L"enableSynchronousStateUpdates"},
      SyncMethod<bool() noexcept>{31, L"enableViewCulling"},
      SyncMethod<bool() noexcept>{32, L"enableViewRecycling"},
      SyncMethod<bool() noexcept>{33, L"enableViewRecyclingForText"},
      SyncMethod<bool() noexcept>{34, L"enableViewRecyclingForView"},
      SyncMethod<bool() noexcept>{35, L"fixMappingOfEventPrioritiesBetweenFabricAndReact"},
      SyncMethod<bool() noexcept>{36, L"fuseboxEnabledRelease"},
      SyncMethod<bool() noexcept>{37, L"fuseboxNetworkInspectionEnabled"},
      SyncMethod<bool() noexcept>{38, L"incorporateMaxLinesDuringAndroidLayout"},
      SyncMethod<bool() noexcept>{39, L"traceTurboModulePromiseRejectionsOnAndroid"},
      SyncMethod<bool() noexcept>{40, L"updateRuntimeShadowNodeReferencesOnCommit"},
      SyncMethod<bool() noexcept>{41, L"useAlwaysAvailableJSErrorHandling"},
      SyncMethod<bool() noexcept>{42, L"useAndroidTextLayoutWidthDirectly"},
      SyncMethod<bool() noexcept>{43, L"useFabricInterop"},
      SyncMethod<bool() noexcept>{44, L"useNativeViewConfigsInBridgelessMode"},
      SyncMethod<bool() noexcept>{45, L"useOptimizedEventBatchingOnAndroid"},
      SyncMethod<bool() noexcept>{46, L"useRawPropsJsiValue"},
      SyncMethod<bool() noexcept>{47, L"useShadowNodeStateOnClone"},
      SyncMethod<bool() noexcept>{48, L"useTurboModuleInterop"},
      SyncMethod<bool() noexcept>{49, L"useTurboModules"},
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
          "avoidCeilingAvailableAndroidTextWidth",
          "    REACT_SYNC_METHOD(avoidCeilingAvailableAndroidTextWidth) bool avoidCeilingAvailableAndroidTextWidth() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(avoidCeilingAvailableAndroidTextWidth) static bool avoidCeilingAvailableAndroidTextWidth() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "cxxNativeAnimatedEnabled",
          "    REACT_SYNC_METHOD(cxxNativeAnimatedEnabled) bool cxxNativeAnimatedEnabled() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(cxxNativeAnimatedEnabled) static bool cxxNativeAnimatedEnabled() noexcept { /* implementation */ }\n");
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
          "enableAccessibilityOrder",
          "    REACT_SYNC_METHOD(enableAccessibilityOrder) bool enableAccessibilityOrder() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAccessibilityOrder) static bool enableAccessibilityOrder() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "enableAccumulatedUpdatesInRawPropsAndroid",
          "    REACT_SYNC_METHOD(enableAccumulatedUpdatesInRawPropsAndroid) bool enableAccumulatedUpdatesInRawPropsAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableAccumulatedUpdatesInRawPropsAndroid) static bool enableAccumulatedUpdatesInRawPropsAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "enableBridgelessArchitecture",
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableBridgelessArchitecture) static bool enableBridgelessArchitecture() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "enableCppPropsIteratorSetter",
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCppPropsIteratorSetter) static bool enableCppPropsIteratorSetter() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "enableCustomFocusSearchOnClippedElementsAndroid",
          "    REACT_SYNC_METHOD(enableCustomFocusSearchOnClippedElementsAndroid) bool enableCustomFocusSearchOnClippedElementsAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableCustomFocusSearchOnClippedElementsAndroid) static bool enableCustomFocusSearchOnClippedElementsAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "enableDestroyShadowTreeRevisionAsync",
          "    REACT_SYNC_METHOD(enableDestroyShadowTreeRevisionAsync) bool enableDestroyShadowTreeRevisionAsync() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDestroyShadowTreeRevisionAsync) static bool enableDestroyShadowTreeRevisionAsync() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "enableDoubleMeasurementFixAndroid",
          "    REACT_SYNC_METHOD(enableDoubleMeasurementFixAndroid) bool enableDoubleMeasurementFixAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableDoubleMeasurementFixAndroid) static bool enableDoubleMeasurementFixAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "enableEagerRootViewAttachment",
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableEagerRootViewAttachment) static bool enableEagerRootViewAttachment() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "enableFabricLogs",
          "    REACT_SYNC_METHOD(enableFabricLogs) bool enableFabricLogs() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricLogs) static bool enableFabricLogs() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "enableFabricRenderer",
          "    REACT_SYNC_METHOD(enableFabricRenderer) bool enableFabricRenderer() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFabricRenderer) static bool enableFabricRenderer() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "enableFixForParentTagDuringReparenting",
          "    REACT_SYNC_METHOD(enableFixForParentTagDuringReparenting) bool enableFixForParentTagDuringReparenting() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFixForParentTagDuringReparenting) static bool enableFixForParentTagDuringReparenting() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "enableFontScaleChangesUpdatingLayout",
          "    REACT_SYNC_METHOD(enableFontScaleChangesUpdatingLayout) bool enableFontScaleChangesUpdatingLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableFontScaleChangesUpdatingLayout) static bool enableFontScaleChangesUpdatingLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "enableIOSViewClipToPaddingBox",
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIOSViewClipToPaddingBox) static bool enableIOSViewClipToPaddingBox() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "enableIntersectionObserverEventLoopIntegration",
          "    REACT_SYNC_METHOD(enableIntersectionObserverEventLoopIntegration) bool enableIntersectionObserverEventLoopIntegration() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableIntersectionObserverEventLoopIntegration) static bool enableIntersectionObserverEventLoopIntegration() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "enableLayoutAnimationsOnAndroid",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnAndroid) static bool enableLayoutAnimationsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "enableLayoutAnimationsOnIOS",
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableLayoutAnimationsOnIOS) static bool enableLayoutAnimationsOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "enableMainQueueModulesOnIOS",
          "    REACT_SYNC_METHOD(enableMainQueueModulesOnIOS) bool enableMainQueueModulesOnIOS() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableMainQueueModulesOnIOS) static bool enableMainQueueModulesOnIOS() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "enableNativeCSSParsing",
          "    REACT_SYNC_METHOD(enableNativeCSSParsing) bool enableNativeCSSParsing() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNativeCSSParsing) static bool enableNativeCSSParsing() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "enableNetworkEventReporting",
          "    REACT_SYNC_METHOD(enableNetworkEventReporting) bool enableNetworkEventReporting() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNetworkEventReporting) static bool enableNetworkEventReporting() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "enableNewBackgroundAndBorderDrawables",
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableNewBackgroundAndBorderDrawables) static bool enableNewBackgroundAndBorderDrawables() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "enablePreparedTextLayout",
          "    REACT_SYNC_METHOD(enablePreparedTextLayout) bool enablePreparedTextLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePreparedTextLayout) static bool enablePreparedTextLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "enablePropsUpdateReconciliationAndroid",
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enablePropsUpdateReconciliationAndroid) static bool enablePropsUpdateReconciliationAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          29,
          "enableResourceTimingAPI",
          "    REACT_SYNC_METHOD(enableResourceTimingAPI) bool enableResourceTimingAPI() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableResourceTimingAPI) static bool enableResourceTimingAPI() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          30,
          "enableSynchronousStateUpdates",
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableSynchronousStateUpdates) static bool enableSynchronousStateUpdates() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          31,
          "enableViewCulling",
          "    REACT_SYNC_METHOD(enableViewCulling) bool enableViewCulling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewCulling) static bool enableViewCulling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "enableViewRecycling",
          "    REACT_SYNC_METHOD(enableViewRecycling) bool enableViewRecycling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecycling) static bool enableViewRecycling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "enableViewRecyclingForText",
          "    REACT_SYNC_METHOD(enableViewRecyclingForText) bool enableViewRecyclingForText() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecyclingForText) static bool enableViewRecyclingForText() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "enableViewRecyclingForView",
          "    REACT_SYNC_METHOD(enableViewRecyclingForView) bool enableViewRecyclingForView() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(enableViewRecyclingForView) static bool enableViewRecyclingForView() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "fixMappingOfEventPrioritiesBetweenFabricAndReact",
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fixMappingOfEventPrioritiesBetweenFabricAndReact) static bool fixMappingOfEventPrioritiesBetweenFabricAndReact() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "fuseboxEnabledRelease",
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxEnabledRelease) static bool fuseboxEnabledRelease() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "fuseboxNetworkInspectionEnabled",
          "    REACT_SYNC_METHOD(fuseboxNetworkInspectionEnabled) bool fuseboxNetworkInspectionEnabled() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(fuseboxNetworkInspectionEnabled) static bool fuseboxNetworkInspectionEnabled() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "incorporateMaxLinesDuringAndroidLayout",
          "    REACT_SYNC_METHOD(incorporateMaxLinesDuringAndroidLayout) bool incorporateMaxLinesDuringAndroidLayout() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(incorporateMaxLinesDuringAndroidLayout) static bool incorporateMaxLinesDuringAndroidLayout() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          39,
          "traceTurboModulePromiseRejectionsOnAndroid",
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(traceTurboModulePromiseRejectionsOnAndroid) static bool traceTurboModulePromiseRejectionsOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          40,
          "updateRuntimeShadowNodeReferencesOnCommit",
          "    REACT_SYNC_METHOD(updateRuntimeShadowNodeReferencesOnCommit) bool updateRuntimeShadowNodeReferencesOnCommit() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(updateRuntimeShadowNodeReferencesOnCommit) static bool updateRuntimeShadowNodeReferencesOnCommit() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          41,
          "useAlwaysAvailableJSErrorHandling",
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useAlwaysAvailableJSErrorHandling) static bool useAlwaysAvailableJSErrorHandling() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          42,
          "useAndroidTextLayoutWidthDirectly",
          "    REACT_SYNC_METHOD(useAndroidTextLayoutWidthDirectly) bool useAndroidTextLayoutWidthDirectly() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useAndroidTextLayoutWidthDirectly) static bool useAndroidTextLayoutWidthDirectly() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          43,
          "useFabricInterop",
          "    REACT_SYNC_METHOD(useFabricInterop) bool useFabricInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useFabricInterop) static bool useFabricInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          44,
          "useNativeViewConfigsInBridgelessMode",
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useNativeViewConfigsInBridgelessMode) static bool useNativeViewConfigsInBridgelessMode() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          45,
          "useOptimizedEventBatchingOnAndroid",
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useOptimizedEventBatchingOnAndroid) static bool useOptimizedEventBatchingOnAndroid() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          46,
          "useRawPropsJsiValue",
          "    REACT_SYNC_METHOD(useRawPropsJsiValue) bool useRawPropsJsiValue() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useRawPropsJsiValue) static bool useRawPropsJsiValue() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          47,
          "useShadowNodeStateOnClone",
          "    REACT_SYNC_METHOD(useShadowNodeStateOnClone) bool useShadowNodeStateOnClone() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useShadowNodeStateOnClone) static bool useShadowNodeStateOnClone() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          48,
          "useTurboModuleInterop",
          "    REACT_SYNC_METHOD(useTurboModuleInterop) bool useTurboModuleInterop() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModuleInterop) static bool useTurboModuleInterop() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          49,
          "useTurboModules",
          "    REACT_SYNC_METHOD(useTurboModules) bool useTurboModules() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(useTurboModules) static bool useTurboModules() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
