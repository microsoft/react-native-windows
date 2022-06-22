
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

REACT_STRUCT(AnimatedTurboModuleSpec_EndResult)
struct AnimatedTurboModuleSpec_EndResult {
    REACT_FIELD(finished)
    bool finished;
};

REACT_STRUCT(AnimatedTurboModuleSpec_EventMapping)
struct AnimatedTurboModuleSpec_EventMapping {
    REACT_FIELD(nativeEventPath)
    std::vector<std::string> nativeEventPath;
    REACT_FIELD(animatedValueTag)
    std::optional<double> animatedValueTag;
};

struct AnimatedTurboModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"startOperationBatch"},
      Method<void() noexcept>{1, L"finishOperationBatch"},
      Method<void(double, ::React::JSValue) noexcept>{2, L"createAnimatedNode"},
      Method<void(double, ::React::JSValue) noexcept>{3, L"updateAnimatedNodeConfig"},
      Method<void(double, Callback<double>) noexcept>{4, L"getValue"},
      Method<void(double) noexcept>{5, L"startListeningToAnimatedNodeValue"},
      Method<void(double) noexcept>{6, L"stopListeningToAnimatedNodeValue"},
      Method<void(double, double) noexcept>{7, L"connectAnimatedNodes"},
      Method<void(double, double) noexcept>{8, L"disconnectAnimatedNodes"},
      Method<void(double, double, ::React::JSValue, Callback<AnimatedTurboModuleSpec_EndResult>) noexcept>{9, L"startAnimatingNode"},
      Method<void(double) noexcept>{10, L"stopAnimation"},
      Method<void(double, double) noexcept>{11, L"setAnimatedNodeValue"},
      Method<void(double, double) noexcept>{12, L"setAnimatedNodeOffset"},
      Method<void(double) noexcept>{13, L"flattenAnimatedNodeOffset"},
      Method<void(double) noexcept>{14, L"extractAnimatedNodeOffset"},
      Method<void(double, double) noexcept>{15, L"connectAnimatedNodeToView"},
      Method<void(double, double) noexcept>{16, L"disconnectAnimatedNodeFromView"},
      Method<void(double) noexcept>{17, L"restoreDefaultValues"},
      Method<void(double) noexcept>{18, L"dropAnimatedNode"},
      Method<void(double, std::string, AnimatedTurboModuleSpec_EventMapping) noexcept>{19, L"addAnimatedEventToView"},
      Method<void(double, std::string, double) noexcept>{20, L"removeAnimatedEventFromView"},
      Method<void(std::string) noexcept>{21, L"addListener"},
      Method<void(double) noexcept>{22, L"removeListeners"},
      Method<void(::React::JSValueArray) noexcept>{23, L"queueAndExecuteBatchedOperations"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AnimatedTurboModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startOperationBatch",
          "    REACT_METHOD(startOperationBatch) void startOperationBatch() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startOperationBatch) static void startOperationBatch() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "finishOperationBatch",
          "    REACT_METHOD(finishOperationBatch) void finishOperationBatch() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(finishOperationBatch) static void finishOperationBatch() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "createAnimatedNode",
          "    REACT_METHOD(createAnimatedNode) void createAnimatedNode(double tag, ::React::JSValue && config) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createAnimatedNode) static void createAnimatedNode(double tag, ::React::JSValue && config) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "updateAnimatedNodeConfig",
          "    REACT_METHOD(updateAnimatedNodeConfig) void updateAnimatedNodeConfig(double tag, ::React::JSValue && config) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(updateAnimatedNodeConfig) static void updateAnimatedNodeConfig(double tag, ::React::JSValue && config) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getValue",
          "    REACT_METHOD(getValue) void getValue(double tag, std::function<void(double)> const & saveValueCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getValue) static void getValue(double tag, std::function<void(double)> const & saveValueCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "startListeningToAnimatedNodeValue",
          "    REACT_METHOD(startListeningToAnimatedNodeValue) void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startListeningToAnimatedNodeValue) static void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "stopListeningToAnimatedNodeValue",
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) static void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "connectAnimatedNodes",
          "    REACT_METHOD(connectAnimatedNodes) void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodes) static void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "disconnectAnimatedNodes",
          "    REACT_METHOD(disconnectAnimatedNodes) void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodes) static void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "startAnimatingNode",
          "    REACT_METHOD(startAnimatingNode) void startAnimatingNode(double animationId, double nodeTag, ::React::JSValue && config, std::function<void(AnimatedTurboModuleSpec_EndResult const &)> const & endCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startAnimatingNode) static void startAnimatingNode(double animationId, double nodeTag, ::React::JSValue && config, std::function<void(AnimatedTurboModuleSpec_EndResult const &)> const & endCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "stopAnimation",
          "    REACT_METHOD(stopAnimation) void stopAnimation(double animationId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopAnimation) static void stopAnimation(double animationId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "setAnimatedNodeValue",
          "    REACT_METHOD(setAnimatedNodeValue) void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeValue) static void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "setAnimatedNodeOffset",
          "    REACT_METHOD(setAnimatedNodeOffset) void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeOffset) static void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "flattenAnimatedNodeOffset",
          "    REACT_METHOD(flattenAnimatedNodeOffset) void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(flattenAnimatedNodeOffset) static void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "extractAnimatedNodeOffset",
          "    REACT_METHOD(extractAnimatedNodeOffset) void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(extractAnimatedNodeOffset) static void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "connectAnimatedNodeToView",
          "    REACT_METHOD(connectAnimatedNodeToView) void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodeToView) static void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "disconnectAnimatedNodeFromView",
          "    REACT_METHOD(disconnectAnimatedNodeFromView) void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodeFromView) static void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "restoreDefaultValues",
          "    REACT_METHOD(restoreDefaultValues) void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(restoreDefaultValues) static void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "dropAnimatedNode",
          "    REACT_METHOD(dropAnimatedNode) void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dropAnimatedNode) static void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "addAnimatedEventToView",
          "    REACT_METHOD(addAnimatedEventToView) void addAnimatedEventToView(double viewTag, std::string eventName, AnimatedTurboModuleSpec_EventMapping && eventMapping) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addAnimatedEventToView) static void addAnimatedEventToView(double viewTag, std::string eventName, AnimatedTurboModuleSpec_EventMapping && eventMapping) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "removeAnimatedEventFromView",
          "    REACT_METHOD(removeAnimatedEventFromView) void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeAnimatedEventFromView) static void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "queueAndExecuteBatchedOperations",
          "    REACT_METHOD(queueAndExecuteBatchedOperations) void queueAndExecuteBatchedOperations(::React::JSValueArray && operationsAndArgs) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(queueAndExecuteBatchedOperations) static void queueAndExecuteBatchedOperations(::React::JSValueArray && operationsAndArgs) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
