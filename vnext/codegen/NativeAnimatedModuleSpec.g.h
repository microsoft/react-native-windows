
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

struct AnimatedModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"startOperationBatch"},
      Method<void() noexcept>{1, L"finishOperationBatch"},
      Method<void(double, React::JSValueObject) noexcept>{2, L"createAnimatedNode"},
      Method<void(double, Callback<React::JSValue>) noexcept>{3, L"getValue"},
      Method<void(double) noexcept>{4, L"startListeningToAnimatedNodeValue"},
      Method<void(double) noexcept>{5, L"stopListeningToAnimatedNodeValue"},
      Method<void(double, double) noexcept>{6, L"connectAnimatedNodes"},
      Method<void(double, double) noexcept>{7, L"disconnectAnimatedNodes"},
      Method<void(double, double, React::JSValueObject, Callback<React::JSValue>) noexcept>{8, L"startAnimatingNode"},
      Method<void(double) noexcept>{9, L"stopAnimation"},
      Method<void(double, double) noexcept>{10, L"setAnimatedNodeValue"},
      Method<void(double, double) noexcept>{11, L"setAnimatedNodeOffset"},
      Method<void(double) noexcept>{12, L"flattenAnimatedNodeOffset"},
      Method<void(double) noexcept>{13, L"extractAnimatedNodeOffset"},
      Method<void(double, double) noexcept>{14, L"connectAnimatedNodeToView"},
      Method<void(double, double) noexcept>{15, L"disconnectAnimatedNodeFromView"},
      Method<void(double) noexcept>{16, L"restoreDefaultValues"},
      Method<void(double) noexcept>{17, L"dropAnimatedNode"},
      Method<void(double, std::string, React::JSValueObject) noexcept>{18, L"addAnimatedEventToView"},
      Method<void(double, std::string, double) noexcept>{19, L"removeAnimatedEventFromView"},
      Method<void(std::string) noexcept>{20, L"addListener"},
      Method<void(double) noexcept>{21, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AnimatedModuleSpec>();

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
          "    REACT_METHOD(createAnimatedNode) void createAnimatedNode(double tag, React::JSValueObject && config) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createAnimatedNode) static void createAnimatedNode(double tag, React::JSValueObject && config) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getValue",
          "    REACT_METHOD(getValue) void getValue(double tag, std::function<void(React::JSValue const &)> const & saveValueCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getValue) static void getValue(double tag, std::function<void(React::JSValue const &)> const & saveValueCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "startListeningToAnimatedNodeValue",
          "    REACT_METHOD(startListeningToAnimatedNodeValue) void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startListeningToAnimatedNodeValue) static void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "stopListeningToAnimatedNodeValue",
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) static void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "connectAnimatedNodes",
          "    REACT_METHOD(connectAnimatedNodes) void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodes) static void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "disconnectAnimatedNodes",
          "    REACT_METHOD(disconnectAnimatedNodes) void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodes) static void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "startAnimatingNode",
          "    REACT_METHOD(startAnimatingNode) void startAnimatingNode(double animationId, double nodeTag, React::JSValueObject && config, std::function<void(React::JSValue const &)> const & endCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startAnimatingNode) static void startAnimatingNode(double animationId, double nodeTag, React::JSValueObject && config, std::function<void(React::JSValue const &)> const & endCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "stopAnimation",
          "    REACT_METHOD(stopAnimation) void stopAnimation(double animationId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopAnimation) static void stopAnimation(double animationId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "setAnimatedNodeValue",
          "    REACT_METHOD(setAnimatedNodeValue) void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeValue) static void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "setAnimatedNodeOffset",
          "    REACT_METHOD(setAnimatedNodeOffset) void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeOffset) static void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "flattenAnimatedNodeOffset",
          "    REACT_METHOD(flattenAnimatedNodeOffset) void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(flattenAnimatedNodeOffset) static void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "extractAnimatedNodeOffset",
          "    REACT_METHOD(extractAnimatedNodeOffset) void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(extractAnimatedNodeOffset) static void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "connectAnimatedNodeToView",
          "    REACT_METHOD(connectAnimatedNodeToView) void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodeToView) static void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "disconnectAnimatedNodeFromView",
          "    REACT_METHOD(disconnectAnimatedNodeFromView) void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodeFromView) static void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "restoreDefaultValues",
          "    REACT_METHOD(restoreDefaultValues) void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(restoreDefaultValues) static void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "dropAnimatedNode",
          "    REACT_METHOD(dropAnimatedNode) void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dropAnimatedNode) static void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "addAnimatedEventToView",
          "    REACT_METHOD(addAnimatedEventToView) void addAnimatedEventToView(double viewTag, std::string eventName, React::JSValueObject && eventMapping) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addAnimatedEventToView) static void addAnimatedEventToView(double viewTag, std::string eventName, React::JSValueObject && eventMapping) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "removeAnimatedEventFromView",
          "    REACT_METHOD(removeAnimatedEventFromView) void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeAnimatedEventFromView) static void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
