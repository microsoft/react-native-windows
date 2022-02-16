
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
      SyncMethod<::React::JSValue(std::string) noexcept>{0, L"getConstantsForViewManager"},
      SyncMethod<std::vector<std::string>() noexcept>{1, L"getDefaultEventTypes"},
      SyncMethod<::React::JSValue(std::string) noexcept>{2, L"lazilyLoadView"},
      Method<void(std::optional<double>, std::string, double, ::React::JSValue) noexcept>{3, L"createView"},
      Method<void(double, std::string, ::React::JSValue) noexcept>{4, L"updateView"},
      Method<void(std::optional<double>) noexcept>{5, L"focus"},
      Method<void(std::optional<double>) noexcept>{6, L"blur"},
      Method<void(std::optional<double>, std::vector<double>, Callback<double, double, double, double, double>) noexcept>{7, L"findSubviewIn"},
      Method<void(std::optional<double>, double, std::optional<::React::JSValueArray>) noexcept>{8, L"dispatchViewManagerCommand"},
      Method<void(std::optional<double>, Callback<double, double, double, double, double, double>) noexcept>{9, L"measure"},
      Method<void(std::optional<double>, Callback<double, double, double, double>) noexcept>{10, L"measureInWindow"},
      Method<void(std::optional<double>, std::optional<double>, Callback<std::vector<bool>>) noexcept>{11, L"viewIsDescendantOf"},
      Method<void(std::optional<double>, std::optional<double>, Callback<::React::JSValue>, Callback<double, double, double, double>) noexcept>{12, L"measureLayout"},
      Method<void(std::optional<double>, Callback<::React::JSValue>, Callback<double, double, double, double>) noexcept>{13, L"measureLayoutRelativeToParent"},
      Method<void(std::optional<double>, bool) noexcept>{14, L"setJSResponder"},
      Method<void() noexcept>{15, L"clearJSResponder"},
      Method<void(::React::JSValue, Callback<>, Callback<::React::JSValue>) noexcept>{16, L"configureNextLayoutAnimation"},
      Method<void(double) noexcept>{17, L"removeSubviewsFromContainerWithID"},
      Method<void(std::optional<double>, std::optional<double>) noexcept>{18, L"replaceExistingNonRootView"},
      Method<void(std::optional<double>, std::vector<double>) noexcept>{19, L"setChildren"},
      Method<void(std::optional<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>) noexcept>{20, L"manageChildren"},
      Method<void(bool) noexcept>{21, L"setLayoutAnimationEnabledExperimental"},
      Method<void(std::optional<double>, double) noexcept>{22, L"sendAccessibilityEvent"},
      Method<void(std::optional<double>, std::vector<std::string>, Callback<::React::JSValue>, Callback<std::string, double>) noexcept>{23, L"showPopupMenu"},
      Method<void() noexcept>{24, L"dismissPopupMenu"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, UIManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getConstantsForViewManager",
          "    REACT_SYNC_METHOD(getConstantsForViewManager) ::React::JSValue getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getConstantsForViewManager) static ::React::JSValue getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getDefaultEventTypes",
          "    REACT_SYNC_METHOD(getDefaultEventTypes) std::vector<std::string> getDefaultEventTypes() noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(getDefaultEventTypes) static std::vector<std::string> getDefaultEventTypes() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "lazilyLoadView",
          "    REACT_SYNC_METHOD(lazilyLoadView) ::React::JSValue lazilyLoadView(std::string name) noexcept { /* implementation */ }}\n"
          "    REACT_SYNC_METHOD(lazilyLoadView) static ::React::JSValue lazilyLoadView(std::string name) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "createView",
          "    REACT_METHOD(createView) void createView(std::optional<double> reactTag, std::string viewName, double rootTag, ::React::JSValue && props) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(createView) static void createView(std::optional<double> reactTag, std::string viewName, double rootTag, ::React::JSValue && props) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "updateView",
          "    REACT_METHOD(updateView) void updateView(double reactTag, std::string viewName, ::React::JSValue && props) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(updateView) static void updateView(double reactTag, std::string viewName, ::React::JSValue && props) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "focus",
          "    REACT_METHOD(focus) void focus(std::optional<double> reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(focus) static void focus(std::optional<double> reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "blur",
          "    REACT_METHOD(blur) void blur(std::optional<double> reactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(blur) static void blur(std::optional<double> reactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "findSubviewIn",
          "    REACT_METHOD(findSubviewIn) void findSubviewIn(std::optional<double> reactTag, std::vector<double> const & point, std::function<void(double, double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(findSubviewIn) static void findSubviewIn(std::optional<double> reactTag, std::vector<double> const & point, std::function<void(double, double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "dispatchViewManagerCommand",
          "    REACT_METHOD(dispatchViewManagerCommand) void dispatchViewManagerCommand(std::optional<double> reactTag, double commandID, std::optional<::React::JSValueArray> commandArgs) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dispatchViewManagerCommand) static void dispatchViewManagerCommand(std::optional<double> reactTag, double commandID, std::optional<::React::JSValueArray> commandArgs) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "measure",
          "    REACT_METHOD(measure) void measure(std::optional<double> reactTag, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measure) static void measure(std::optional<double> reactTag, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "measureInWindow",
          "    REACT_METHOD(measureInWindow) void measureInWindow(std::optional<double> reactTag, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureInWindow) static void measureInWindow(std::optional<double> reactTag, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "viewIsDescendantOf",
          "    REACT_METHOD(viewIsDescendantOf) void viewIsDescendantOf(std::optional<double> reactTag, std::optional<double> ancestorReactTag, std::function<void(std::vector<bool> const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(viewIsDescendantOf) static void viewIsDescendantOf(std::optional<double> reactTag, std::optional<double> ancestorReactTag, std::function<void(std::vector<bool> const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "measureLayout",
          "    REACT_METHOD(measureLayout) void measureLayout(std::optional<double> reactTag, std::optional<double> ancestorReactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureLayout) static void measureLayout(std::optional<double> reactTag, std::optional<double> ancestorReactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "measureLayoutRelativeToParent",
          "    REACT_METHOD(measureLayoutRelativeToParent) void measureLayoutRelativeToParent(std::optional<double> reactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(measureLayoutRelativeToParent) static void measureLayoutRelativeToParent(std::optional<double> reactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "setJSResponder",
          "    REACT_METHOD(setJSResponder) void setJSResponder(std::optional<double> reactTag, bool blockNativeResponder) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setJSResponder) static void setJSResponder(std::optional<double> reactTag, bool blockNativeResponder) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "clearJSResponder",
          "    REACT_METHOD(clearJSResponder) void clearJSResponder() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clearJSResponder) static void clearJSResponder() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "configureNextLayoutAnimation",
          "    REACT_METHOD(configureNextLayoutAnimation) void configureNextLayoutAnimation(::React::JSValue && config, std::function<void()> const & callback, std::function<void(::React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(configureNextLayoutAnimation) static void configureNextLayoutAnimation(::React::JSValue && config, std::function<void()> const & callback, std::function<void(::React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "removeSubviewsFromContainerWithID",
          "    REACT_METHOD(removeSubviewsFromContainerWithID) void removeSubviewsFromContainerWithID(double containerID) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeSubviewsFromContainerWithID) static void removeSubviewsFromContainerWithID(double containerID) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "replaceExistingNonRootView",
          "    REACT_METHOD(replaceExistingNonRootView) void replaceExistingNonRootView(std::optional<double> reactTag, std::optional<double> newReactTag) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(replaceExistingNonRootView) static void replaceExistingNonRootView(std::optional<double> reactTag, std::optional<double> newReactTag) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "setChildren",
          "    REACT_METHOD(setChildren) void setChildren(std::optional<double> containerTag, std::vector<double> const & reactTags) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setChildren) static void setChildren(std::optional<double> containerTag, std::vector<double> const & reactTags) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "manageChildren",
          "    REACT_METHOD(manageChildren) void manageChildren(std::optional<double> containerTag, std::vector<double> const & moveFromIndices, std::vector<double> const & moveToIndices, std::vector<double> const & addChildReactTags, std::vector<double> const & addAtIndices, std::vector<double> const & removeAtIndices) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(manageChildren) static void manageChildren(std::optional<double> containerTag, std::vector<double> const & moveFromIndices, std::vector<double> const & moveToIndices, std::vector<double> const & addChildReactTags, std::vector<double> const & addAtIndices, std::vector<double> const & removeAtIndices) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "setLayoutAnimationEnabledExperimental",
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) static void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "sendAccessibilityEvent",
          "    REACT_METHOD(sendAccessibilityEvent) void sendAccessibilityEvent(std::optional<double> reactTag, double eventType) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(sendAccessibilityEvent) static void sendAccessibilityEvent(std::optional<double> reactTag, double eventType) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "showPopupMenu",
          "    REACT_METHOD(showPopupMenu) void showPopupMenu(std::optional<double> reactTag, std::vector<std::string> const & items, std::function<void(::React::JSValue const &)> const & error, std::function<void(std::string, double)> const & success) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(showPopupMenu) static void showPopupMenu(std::optional<double> reactTag, std::vector<std::string> const & items, std::function<void(::React::JSValue const &)> const & error, std::function<void(std::string, double)> const & success) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "dismissPopupMenu",
          "    REACT_METHOD(dismissPopupMenu) void dismissPopupMenu() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismissPopupMenu) static void dismissPopupMenu() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
