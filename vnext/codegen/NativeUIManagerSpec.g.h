
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

struct UIManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<React::JSValueObject(std::string) noexcept>{0, L"getConstantsForViewManager"},
      SyncMethod<React::JSValueArray() noexcept>{1, L"getDefaultEventTypes"},
      SyncMethod<React::JSValueObject(std::string) noexcept>{2, L"lazilyLoadView"},
      Method<void(double, std::string, double, React::JSValueObject) noexcept>{3, L"createView"},
      Method<void(double, std::string, React::JSValueObject) noexcept>{4, L"updateView"},
      Method<void(double) noexcept>{5, L"focus"},
      Method<void(double) noexcept>{6, L"blur"},
      Method<void(double, React::JSValueArray, Callback<React::JSValue>) noexcept>{7, L"findSubviewIn"},
      Method<void(double, double, React::JSValueArray) noexcept>{8, L"dispatchViewManagerCommand"},
      Method<void(double, Callback<React::JSValue>) noexcept>{9, L"measure"},
      Method<void(double, Callback<React::JSValue>) noexcept>{10, L"measureInWindow"},
      Method<void(double, double, Callback<React::JSValue>) noexcept>{11, L"viewIsDescendantOf"},
      Method<void(double, double, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{12, L"measureLayout"},
      Method<void(double, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{13, L"measureLayoutRelativeToParent"},
      Method<void(double, bool) noexcept>{14, L"setJSResponder"},
      Method<void() noexcept>{15, L"clearJSResponder"},
      Method<void(React::JSValueObject, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{16, L"configureNextLayoutAnimation"},
      Method<void(double) noexcept>{17, L"removeSubviewsFromContainerWithID"},
      Method<void(double, double) noexcept>{18, L"replaceExistingNonRootView"},
      Method<void(double, React::JSValueArray) noexcept>{19, L"setChildren"},
      Method<void(double, React::JSValueArray, React::JSValueArray, React::JSValueArray, React::JSValueArray, React::JSValueArray) noexcept>{20, L"manageChildren"},
      Method<void(bool) noexcept>{21, L"setLayoutAnimationEnabledExperimental"},
      Method<void(double, double) noexcept>{22, L"sendAccessibilityEvent"},
      Method<void(double, React::JSValueArray, Callback<React::JSValue>, Callback<React::JSValue>) noexcept>{23, L"showPopupMenu"},
      Method<void() noexcept>{24, L"dismissPopupMenu"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, UIManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getConstantsForViewManager",
          "    REACT_SYNC_METHOD(getConstantsForViewManager) React::JSValueObject getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getConstantsForViewManager) static React::JSValueObject getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getDefaultEventTypes",
          "    REACT_SYNC_METHOD(getDefaultEventTypes) React::JSValueArray getDefaultEventTypes() noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getDefaultEventTypes) static React::JSValueArray getDefaultEventTypes() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "lazilyLoadView",
          "    REACT_SYNC_METHOD(lazilyLoadView) React::JSValueObject lazilyLoadView(std::string name) noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(lazilyLoadView) static React::JSValueObject lazilyLoadView(std::string name) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "createView",
          "    REACT_METHOD(createView) void createView(double reactTag, std::string viewName, double rootTag, React::JSValueObject && props) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createView) static void createView(double reactTag, std::string viewName, double rootTag, React::JSValueObject && props) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "updateView",
          "    REACT_METHOD(updateView) void updateView(double reactTag, std::string viewName, React::JSValueObject && props) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(updateView) static void updateView(double reactTag, std::string viewName, React::JSValueObject && props) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "focus",
          "    REACT_METHOD(focus) void focus(double reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(focus) static void focus(double reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "blur",
          "    REACT_METHOD(blur) void blur(double reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(blur) static void blur(double reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "findSubviewIn",
          "    REACT_METHOD(findSubviewIn) void findSubviewIn(double reactTag, React::JSValueArray && point, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(findSubviewIn) static void findSubviewIn(double reactTag, React::JSValueArray && point, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "dispatchViewManagerCommand",
          "    REACT_METHOD(dispatchViewManagerCommand) void dispatchViewManagerCommand(double reactTag, double commandID, React::JSValueArray && commandArgs) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dispatchViewManagerCommand) static void dispatchViewManagerCommand(double reactTag, double commandID, React::JSValueArray && commandArgs) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "measure",
          "    REACT_METHOD(measure) void measure(double reactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measure) static void measure(double reactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "measureInWindow",
          "    REACT_METHOD(measureInWindow) void measureInWindow(double reactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureInWindow) static void measureInWindow(double reactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "viewIsDescendantOf",
          "    REACT_METHOD(viewIsDescendantOf) void viewIsDescendantOf(double reactTag, double ancestorReactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(viewIsDescendantOf) static void viewIsDescendantOf(double reactTag, double ancestorReactTag, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "measureLayout",
          "    REACT_METHOD(measureLayout) void measureLayout(double reactTag, double ancestorReactTag, std::function<void(React::JSValue const &)> const & errorCallback, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureLayout) static void measureLayout(double reactTag, double ancestorReactTag, std::function<void(React::JSValue const &)> const & errorCallback, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "measureLayoutRelativeToParent",
          "    REACT_METHOD(measureLayoutRelativeToParent) void measureLayoutRelativeToParent(double reactTag, std::function<void(React::JSValue const &)> const & errorCallback, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureLayoutRelativeToParent) static void measureLayoutRelativeToParent(double reactTag, std::function<void(React::JSValue const &)> const & errorCallback, std::function<void(React::JSValue const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "setJSResponder",
          "    REACT_METHOD(setJSResponder) void setJSResponder(double reactTag, bool blockNativeResponder) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setJSResponder) static void setJSResponder(double reactTag, bool blockNativeResponder) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "clearJSResponder",
          "    REACT_METHOD(clearJSResponder) void clearJSResponder() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clearJSResponder) static void clearJSResponder() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "configureNextLayoutAnimation",
          "    REACT_METHOD(configureNextLayoutAnimation) void configureNextLayoutAnimation(React::JSValueObject && config, std::function<void(React::JSValue const &)> const & callback, std::function<void(React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(configureNextLayoutAnimation) static void configureNextLayoutAnimation(React::JSValueObject && config, std::function<void(React::JSValue const &)> const & callback, std::function<void(React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "removeSubviewsFromContainerWithID",
          "    REACT_METHOD(removeSubviewsFromContainerWithID) void removeSubviewsFromContainerWithID(double containerID) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeSubviewsFromContainerWithID) static void removeSubviewsFromContainerWithID(double containerID) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "replaceExistingNonRootView",
          "    REACT_METHOD(replaceExistingNonRootView) void replaceExistingNonRootView(double reactTag, double newReactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(replaceExistingNonRootView) static void replaceExistingNonRootView(double reactTag, double newReactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "setChildren",
          "    REACT_METHOD(setChildren) void setChildren(double containerTag, React::JSValueArray && reactTags) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setChildren) static void setChildren(double containerTag, React::JSValueArray && reactTags) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "manageChildren",
          "    REACT_METHOD(manageChildren) void manageChildren(double containerTag, React::JSValueArray && moveFromIndices, React::JSValueArray && moveToIndices, React::JSValueArray && addChildReactTags, React::JSValueArray && addAtIndices, React::JSValueArray && removeAtIndices) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(manageChildren) static void manageChildren(double containerTag, React::JSValueArray && moveFromIndices, React::JSValueArray && moveToIndices, React::JSValueArray && addChildReactTags, React::JSValueArray && addAtIndices, React::JSValueArray && removeAtIndices) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "setLayoutAnimationEnabledExperimental",
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) static void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "sendAccessibilityEvent",
          "    REACT_METHOD(sendAccessibilityEvent) void sendAccessibilityEvent(double reactTag, double eventType) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(sendAccessibilityEvent) static void sendAccessibilityEvent(double reactTag, double eventType) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "showPopupMenu",
          "    REACT_METHOD(showPopupMenu) void showPopupMenu(double reactTag, React::JSValueArray && items, std::function<void(React::JSValue const &)> const & error, std::function<void(React::JSValue const &)> const & success) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showPopupMenu) static void showPopupMenu(double reactTag, React::JSValueArray && items, std::function<void(React::JSValue const &)> const & error, std::function<void(React::JSValue const &)> const & success) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "dismissPopupMenu",
          "    REACT_METHOD(dismissPopupMenu) void dismissPopupMenu() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismissPopupMenu) static void dismissPopupMenu() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
