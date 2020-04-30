
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
      Method<void(double, JSValueObject) noexcept>{0, L"createAnimatedNode"},
      Method<void(double) noexcept>{1, L"startListeningToAnimatedNodeValue"},
      Method<void(double) noexcept>{2, L"stopListeningToAnimatedNodeValue"},
      Method<void(double, double) noexcept>{3, L"connectAnimatedNodes"},
      Method<void(double, double) noexcept>{4, L"disconnectAnimatedNodes"},
      Method<void(double, double, JSValueObject, Callback<JSValue>) noexcept>{5, L"startAnimatingNode"},
      Method<void(double) noexcept>{6, L"stopAnimation"},
      Method<void(double, double) noexcept>{7, L"setAnimatedNodeValue"},
      Method<void(double, double) noexcept>{8, L"setAnimatedNodeOffset"},
      Method<void(double) noexcept>{9, L"flattenAnimatedNodeOffset"},
      Method<void(double) noexcept>{10, L"extractAnimatedNodeOffset"},
      Method<void(double, double) noexcept>{11, L"connectAnimatedNodeToView"},
      Method<void(double, double) noexcept>{12, L"disconnectAnimatedNodeFromView"},
      Method<void(double) noexcept>{13, L"restoreDefaultValues"},
      Method<void(double) noexcept>{14, L"dropAnimatedNode"},
      Method<void(double, std::string, JSValueObject) noexcept>{15, L"addAnimatedEventToView"},
      Method<void(double, std::string, double) noexcept>{16, L"removeAnimatedEventFromView"},
      Method<void(std::string) noexcept>{17, L"addListener"},
      Method<void(double) noexcept>{18, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AnimatedModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "createAnimatedNode",
          "    REACT_METHOD(createAnimatedNode) void createAnimatedNode(double tag, React::JSValueObject && config) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createAnimatedNode) static void createAnimatedNode(double tag, React::JSValueObject && config) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "startListeningToAnimatedNodeValue",
          "    REACT_METHOD(startListeningToAnimatedNodeValue) void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startListeningToAnimatedNodeValue) static void startListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "stopListeningToAnimatedNodeValue",
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopListeningToAnimatedNodeValue) static void stopListeningToAnimatedNodeValue(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "connectAnimatedNodes",
          "    REACT_METHOD(connectAnimatedNodes) void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodes) static void connectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "disconnectAnimatedNodes",
          "    REACT_METHOD(disconnectAnimatedNodes) void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodes) static void disconnectAnimatedNodes(double parentTag, double childTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "startAnimatingNode",
          "    REACT_METHOD(startAnimatingNode) void startAnimatingNode(double animationId, double nodeTag, React::JSValueObject && config, std::function<void(React::JSValue const &)> const & endCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startAnimatingNode) static void startAnimatingNode(double animationId, double nodeTag, React::JSValueObject && config, std::function<void(React::JSValue const &)> const & endCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "stopAnimation",
          "    REACT_METHOD(stopAnimation) void stopAnimation(double animationId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(stopAnimation) static void stopAnimation(double animationId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "setAnimatedNodeValue",
          "    REACT_METHOD(setAnimatedNodeValue) void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeValue) static void setAnimatedNodeValue(double nodeTag, double value) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "setAnimatedNodeOffset",
          "    REACT_METHOD(setAnimatedNodeOffset) void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setAnimatedNodeOffset) static void setAnimatedNodeOffset(double nodeTag, double offset) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "flattenAnimatedNodeOffset",
          "    REACT_METHOD(flattenAnimatedNodeOffset) void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(flattenAnimatedNodeOffset) static void flattenAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "extractAnimatedNodeOffset",
          "    REACT_METHOD(extractAnimatedNodeOffset) void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(extractAnimatedNodeOffset) static void extractAnimatedNodeOffset(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "connectAnimatedNodeToView",
          "    REACT_METHOD(connectAnimatedNodeToView) void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(connectAnimatedNodeToView) static void connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "disconnectAnimatedNodeFromView",
          "    REACT_METHOD(disconnectAnimatedNodeFromView) void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(disconnectAnimatedNodeFromView) static void disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "restoreDefaultValues",
          "    REACT_METHOD(restoreDefaultValues) void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(restoreDefaultValues) static void restoreDefaultValues(double nodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "dropAnimatedNode",
          "    REACT_METHOD(dropAnimatedNode) void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dropAnimatedNode) static void dropAnimatedNode(double tag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "addAnimatedEventToView",
          "    REACT_METHOD(addAnimatedEventToView) void addAnimatedEventToView(double viewTag, std::string eventName, React::JSValueObject && eventMapping) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addAnimatedEventToView) static void addAnimatedEventToView(double viewTag, std::string eventName, React::JSValueObject && eventMapping) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "removeAnimatedEventFromView",
          "    REACT_METHOD(removeAnimatedEventFromView) void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeAnimatedEventFromView) static void removeAnimatedEventFromView(double viewTag, std::string eventName, double animatedNodeTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventName) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
