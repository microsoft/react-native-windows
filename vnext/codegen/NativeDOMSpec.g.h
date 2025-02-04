
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


struct DOMSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<() noexcept>{0, L"getParentNode"},
      SyncMethod<std::vector<>() noexcept>{1, L"getChildNodes"},
      SyncMethod<bool() noexcept>{2, L"isConnected"},
      SyncMethod<double(, ) noexcept>{3, L"compareDocumentPosition"},
      SyncMethod<std::string() noexcept>{4, L"getTextContent"},
      SyncMethod<std::vector<double>(, bool) noexcept>{5, L"getBoundingClientRect"},
      SyncMethod<std::vector<>() noexcept>{6, L"getOffset"},
      SyncMethod<std::vector<double>() noexcept>{7, L"getScrollPosition"},
      SyncMethod<std::vector<double>() noexcept>{8, L"getScrollSize"},
      SyncMethod<std::vector<double>() noexcept>{9, L"getInnerSize"},
      SyncMethod<std::vector<double>() noexcept>{10, L"getBorderWidth"},
      SyncMethod<std::string() noexcept>{11, L"getTagName"},
      SyncMethod<bool(, double) noexcept>{12, L"hasPointerCapture"},
      Method<void(, double) noexcept>{13, L"setPointerCapture"},
      Method<void(, double) noexcept>{14, L"releasePointerCapture"},
      Method<void(, Callback<double, double, double, double, double, double>) noexcept>{15, L"measure"},
      Method<void(, Callback<double, double, double, double>) noexcept>{16, L"measureInWindow"},
      Method<void(, , Callback<>, Callback<double, double, double, double>) noexcept>{17, L"measureLayout"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DOMSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getParentNode",
          "    REACT_SYNC_METHOD(getParentNode)  getParentNode( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getParentNode) static  getParentNode( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getChildNodes",
          "    REACT_SYNC_METHOD(getChildNodes) std::vector<> getChildNodes( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getChildNodes) static std::vector<> getChildNodes( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "isConnected",
          "    REACT_SYNC_METHOD(isConnected) bool isConnected( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(isConnected) static bool isConnected( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "compareDocumentPosition",
          "    REACT_SYNC_METHOD(compareDocumentPosition) double compareDocumentPosition( shadowNode,  otherShadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(compareDocumentPosition) static double compareDocumentPosition( shadowNode,  otherShadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getTextContent",
          "    REACT_SYNC_METHOD(getTextContent) std::string getTextContent( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getTextContent) static std::string getTextContent( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getBoundingClientRect",
          "    REACT_SYNC_METHOD(getBoundingClientRect) std::vector<double> getBoundingClientRect( shadowNode, bool includeTransform) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBoundingClientRect) static std::vector<double> getBoundingClientRect( shadowNode, bool includeTransform) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getOffset",
          "    REACT_SYNC_METHOD(getOffset) std::vector<> getOffset( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getOffset) static std::vector<> getOffset( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getScrollPosition",
          "    REACT_SYNC_METHOD(getScrollPosition) std::vector<double> getScrollPosition( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollPosition) static std::vector<double> getScrollPosition( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getScrollSize",
          "    REACT_SYNC_METHOD(getScrollSize) std::vector<double> getScrollSize( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollSize) static std::vector<double> getScrollSize( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "getInnerSize",
          "    REACT_SYNC_METHOD(getInnerSize) std::vector<double> getInnerSize( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getInnerSize) static std::vector<double> getInnerSize( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getBorderWidth",
          "    REACT_SYNC_METHOD(getBorderWidth) std::vector<double> getBorderWidth( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBorderWidth) static std::vector<double> getBorderWidth( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "getTagName",
          "    REACT_SYNC_METHOD(getTagName) std::string getTagName( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getTagName) static std::string getTagName( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "hasPointerCapture",
          "    REACT_SYNC_METHOD(hasPointerCapture) bool hasPointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(hasPointerCapture) static bool hasPointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "setPointerCapture",
          "    REACT_METHOD(setPointerCapture) void setPointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setPointerCapture) static void setPointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "releasePointerCapture",
          "    REACT_METHOD(releasePointerCapture) void releasePointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(releasePointerCapture) static void releasePointerCapture( shadowNode, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "measure",
          "    REACT_METHOD(measure) void measure( shadowNode, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure( shadowNode, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "measureInWindow",
          "    REACT_METHOD(measureInWindow) void measureInWindow( shadowNode, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureInWindow) static void measureInWindow( shadowNode, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "measureLayout",
          "    REACT_METHOD(measureLayout) void measureLayout( shadowNode,  relativeNode, std::function<void()> const & onFail, std::function<void(double, double, double, double)> const & onSuccess) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureLayout) static void measureLayout( shadowNode,  relativeNode, std::function<void()> const & onFail, std::function<void(double, double, double, double)> const & onSuccess) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
