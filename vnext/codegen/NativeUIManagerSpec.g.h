
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


struct UIManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(double, std::string, double, ::React::JSValue) noexcept>{0, L"createView"},
      Method<void(double, std::string, ::React::JSValue) noexcept>{1, L"updateView"},
      Method<void(double, std::vector<double>, Callback<double, double, double, double, double>) noexcept>{2, L"findSubviewIn"},
      Method<void(double, double, ::React::JSValueArray) noexcept>{3, L"dispatchViewManagerCommand"},
      Method<void(double, Callback<double, double, double, double, double, double>) noexcept>{4, L"measure"},
      Method<void(double, Callback<double, double, double, double>) noexcept>{5, L"measureInWindow"},
      Method<void(double, double, Callback<std::vector<bool>>) noexcept>{6, L"viewIsDescendantOf"},
      Method<void(double, double, Callback<::React::JSValue>, Callback<double, double, double, double>) noexcept>{7, L"measureLayout"},
      Method<void(double, Callback<::React::JSValue>, Callback<double, double, double, double>) noexcept>{8, L"measureLayoutRelativeToParent"},
      Method<void(double, bool) noexcept>{9, L"setJSResponder"},
      Method<void() noexcept>{10, L"clearJSResponder"},
      Method<void(::React::JSValue, Callback<>, Callback<::React::JSValue>) noexcept>{11, L"configureNextLayoutAnimation"},
      Method<void(double, std::vector<double>) noexcept>{12, L"setChildren"},
      Method<void(double, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>, std::vector<double>) noexcept>{13, L"manageChildren"},
      SyncMethod<std::optional<::React::JSValue>(std::string) noexcept>{14, L"getConstantsForViewManager"},
      SyncMethod<std::vector<std::string>() noexcept>{15, L"getDefaultEventTypes"},
      Method<void(bool) noexcept>{16, L"setLayoutAnimationEnabledExperimental"},
      Method<void(double, double) noexcept>{17, L"sendAccessibilityEvent"},
      SyncMethod<::React::JSValue(std::string) noexcept>{18, L"lazilyLoadView"},
      Method<void(double) noexcept>{19, L"focus"},
      Method<void(double) noexcept>{20, L"blur"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, UIManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "createView",
          "    REACT_METHOD(createView) void createView(double reactTag, std::string viewName, double rootTag, ::React::JSValue && props) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(createView) static void createView(double reactTag, std::string viewName, double rootTag, ::React::JSValue && props) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "updateView",
          "    REACT_METHOD(updateView) void updateView(double reactTag, std::string viewName, ::React::JSValue && props) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(updateView) static void updateView(double reactTag, std::string viewName, ::React::JSValue && props) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "findSubviewIn",
          "    REACT_METHOD(findSubviewIn) void findSubviewIn(double reactTag, std::vector<double> const & point, std::function<void(double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(findSubviewIn) static void findSubviewIn(double reactTag, std::vector<double> const & point, std::function<void(double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "dispatchViewManagerCommand",
          "    REACT_METHOD(dispatchViewManagerCommand) void dispatchViewManagerCommand(double reactTag, double commandID, ::React::JSValueArray && commandArgs) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(dispatchViewManagerCommand) static void dispatchViewManagerCommand(double reactTag, double commandID, ::React::JSValueArray && commandArgs) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "measure",
          "    REACT_METHOD(measure) void measure(double reactTag, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure(double reactTag, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "measureInWindow",
          "    REACT_METHOD(measureInWindow) void measureInWindow(double reactTag, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureInWindow) static void measureInWindow(double reactTag, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "viewIsDescendantOf",
          "    REACT_METHOD(viewIsDescendantOf) void viewIsDescendantOf(double reactTag, double ancestorReactTag, std::function<void(std::vector<bool> const &)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(viewIsDescendantOf) static void viewIsDescendantOf(double reactTag, double ancestorReactTag, std::function<void(std::vector<bool> const &)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "measureLayout",
          "    REACT_METHOD(measureLayout) void measureLayout(double reactTag, double ancestorReactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureLayout) static void measureLayout(double reactTag, double ancestorReactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "measureLayoutRelativeToParent",
          "    REACT_METHOD(measureLayoutRelativeToParent) void measureLayoutRelativeToParent(double reactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureLayoutRelativeToParent) static void measureLayoutRelativeToParent(double reactTag, std::function<void(::React::JSValue const &)> const & errorCallback, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "setJSResponder",
          "    REACT_METHOD(setJSResponder) void setJSResponder(double reactTag, bool blockNativeResponder) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setJSResponder) static void setJSResponder(double reactTag, bool blockNativeResponder) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "clearJSResponder",
          "    REACT_METHOD(clearJSResponder) void clearJSResponder() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(clearJSResponder) static void clearJSResponder() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "configureNextLayoutAnimation",
          "    REACT_METHOD(configureNextLayoutAnimation) void configureNextLayoutAnimation(::React::JSValue && config, std::function<void()> const & callback, std::function<void(::React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(configureNextLayoutAnimation) static void configureNextLayoutAnimation(::React::JSValue && config, std::function<void()> const & callback, std::function<void(::React::JSValue const &)> const & errorCallback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "setChildren",
          "    REACT_METHOD(setChildren) void setChildren(double containerTag, std::vector<double> const & reactTags) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setChildren) static void setChildren(double containerTag, std::vector<double> const & reactTags) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "manageChildren",
          "    REACT_METHOD(manageChildren) void manageChildren(double containerTag, std::vector<double> const & moveFromIndices, std::vector<double> const & moveToIndices, std::vector<double> const & addChildReactTags, std::vector<double> const & addAtIndices, std::vector<double> const & removeAtIndices) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(manageChildren) static void manageChildren(double containerTag, std::vector<double> const & moveFromIndices, std::vector<double> const & moveToIndices, std::vector<double> const & addChildReactTags, std::vector<double> const & addAtIndices, std::vector<double> const & removeAtIndices) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "getConstantsForViewManager",
          "    REACT_SYNC_METHOD(getConstantsForViewManager) std::optional<::React::JSValue> getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getConstantsForViewManager) static std::optional<::React::JSValue> getConstantsForViewManager(std::string viewManagerName) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "getDefaultEventTypes",
          "    REACT_SYNC_METHOD(getDefaultEventTypes) std::vector<std::string> getDefaultEventTypes() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getDefaultEventTypes) static std::vector<std::string> getDefaultEventTypes() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "setLayoutAnimationEnabledExperimental",
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setLayoutAnimationEnabledExperimental) static void setLayoutAnimationEnabledExperimental(bool enabled) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "sendAccessibilityEvent",
          "    REACT_METHOD(sendAccessibilityEvent) void sendAccessibilityEvent(double reactTag, double eventType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(sendAccessibilityEvent) static void sendAccessibilityEvent(double reactTag, double eventType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "lazilyLoadView",
          "    REACT_SYNC_METHOD(lazilyLoadView) ::React::JSValue lazilyLoadView(std::string name) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(lazilyLoadView) static ::React::JSValue lazilyLoadView(std::string name) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "focus",
          "    REACT_METHOD(focus) void focus(double reactTag) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(focus) static void focus(double reactTag) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "blur",
          "    REACT_METHOD(blur) void blur(double reactTag) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(blur) static void blur(double reactTag) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
