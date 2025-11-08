
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
      SyncMethod<double(, ) noexcept>{0, L"compareDocumentPosition"},
      SyncMethod<std::vector<>() noexcept>{1, L"getChildNodes"},
      SyncMethod<(, std::string) noexcept>{2, L"getElementById"},
      SyncMethod<() noexcept>{3, L"getParentNode"},
      SyncMethod<bool() noexcept>{4, L"isConnected"},
      SyncMethod<std::vector<double>() noexcept>{5, L"getBorderWidth"},
      SyncMethod<std::vector<double>(, bool) noexcept>{6, L"getBoundingClientRect"},
      SyncMethod<std::vector<double>() noexcept>{7, L"getInnerSize"},
      SyncMethod<std::vector<double>() noexcept>{8, L"getScrollPosition"},
      SyncMethod<std::vector<double>() noexcept>{9, L"getScrollSize"},
      SyncMethod<std::string() noexcept>{10, L"getTagName"},
      SyncMethod<std::string() noexcept>{11, L"getTextContent"},
      SyncMethod<bool(, double) noexcept>{12, L"hasPointerCapture"},
      Method<void(, double) noexcept>{13, L"releasePointerCapture"},
      Method<void(, double) noexcept>{14, L"setPointerCapture"},
      SyncMethod<std::vector<>() noexcept>{15, L"getOffset"},
      SyncMethod<(double, ) noexcept>{16, L"linkRootNode"},
      Method<void(, Callback<double, double, double, double, double, double>) noexcept>{17, L"measure"},
      Method<void(, Callback<double, double, double, double>) noexcept>{18, L"measureInWindow"},
      Method<void(, , Callback<>, Callback<double, double, double, double>) noexcept>{19, L"measureLayout"},
      Method<void(, ) noexcept>{20, L"setNativeProps"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DOMSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "compareDocumentPosition",
          "    REACT_SYNC_METHOD(compareDocumentPosition) double compareDocumentPosition( nativeNodeReference,  otherNativeNodeReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(compareDocumentPosition) static double compareDocumentPosition( nativeNodeReference,  otherNativeNodeReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getChildNodes",
          "    REACT_SYNC_METHOD(getChildNodes) std::vector<> getChildNodes( nativeNodeReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getChildNodes) static std::vector<> getChildNodes( nativeNodeReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getElementById",
          "    REACT_SYNC_METHOD(getElementById)  getElementById( nativeNodeReference, std::string id) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getElementById) static  getElementById( nativeNodeReference, std::string id) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getParentNode",
          "    REACT_SYNC_METHOD(getParentNode)  getParentNode( nativeNodeReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getParentNode) static  getParentNode( nativeNodeReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "isConnected",
          "    REACT_SYNC_METHOD(isConnected) bool isConnected( nativeNodeReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(isConnected) static bool isConnected( nativeNodeReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getBorderWidth",
          "    REACT_SYNC_METHOD(getBorderWidth) std::vector<double> getBorderWidth( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBorderWidth) static std::vector<double> getBorderWidth( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getBoundingClientRect",
          "    REACT_SYNC_METHOD(getBoundingClientRect) std::vector<double> getBoundingClientRect( nativeElementReference, bool includeTransform) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBoundingClientRect) static std::vector<double> getBoundingClientRect( nativeElementReference, bool includeTransform) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getInnerSize",
          "    REACT_SYNC_METHOD(getInnerSize) std::vector<double> getInnerSize( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getInnerSize) static std::vector<double> getInnerSize( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getScrollPosition",
          "    REACT_SYNC_METHOD(getScrollPosition) std::vector<double> getScrollPosition( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollPosition) static std::vector<double> getScrollPosition( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "getScrollSize",
          "    REACT_SYNC_METHOD(getScrollSize) std::vector<double> getScrollSize( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollSize) static std::vector<double> getScrollSize( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getTagName",
          "    REACT_SYNC_METHOD(getTagName) std::string getTagName( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getTagName) static std::string getTagName( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "getTextContent",
          "    REACT_SYNC_METHOD(getTextContent) std::string getTextContent( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getTextContent) static std::string getTextContent( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "hasPointerCapture",
          "    REACT_SYNC_METHOD(hasPointerCapture) bool hasPointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(hasPointerCapture) static bool hasPointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "releasePointerCapture",
          "    REACT_METHOD(releasePointerCapture) void releasePointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(releasePointerCapture) static void releasePointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "setPointerCapture",
          "    REACT_METHOD(setPointerCapture) void setPointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setPointerCapture) static void setPointerCapture( nativeElementReference, double pointerId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "getOffset",
          "    REACT_SYNC_METHOD(getOffset) std::vector<> getOffset( nativeElementReference) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getOffset) static std::vector<> getOffset( nativeElementReference) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "linkRootNode",
          "    REACT_SYNC_METHOD(linkRootNode)  linkRootNode(double rootTag,  instanceHandle) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(linkRootNode) static  linkRootNode(double rootTag,  instanceHandle) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "measure",
          "    REACT_METHOD(measure) void measure( nativeElementReference, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measure) static void measure( nativeElementReference, std::function<void(double, double, double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "measureInWindow",
          "    REACT_METHOD(measureInWindow) void measureInWindow( nativeElementReference, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureInWindow) static void measureInWindow( nativeElementReference, std::function<void(double, double, double, double)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "measureLayout",
          "    REACT_METHOD(measureLayout) void measureLayout( nativeElementReference,  relativeNode, std::function<void()> const & onFail, std::function<void(double, double, double, double)> const & onSuccess) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(measureLayout) static void measureLayout( nativeElementReference,  relativeNode, std::function<void()> const & onFail, std::function<void(double, double, double, double)> const & onSuccess) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "setNativeProps",
          "    REACT_METHOD(setNativeProps) void setNativeProps( nativeElementReference,  updatePayload) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setNativeProps) static void setNativeProps( nativeElementReference,  updatePayload) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
