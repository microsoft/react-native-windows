
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {


struct DOMSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      SyncMethod<std::optional<>() noexcept>{0, L"getParentNode"},
      SyncMethod<std::optional<std::vector<>>() noexcept>{1, L"getChildNodes"},
      SyncMethod<bool() noexcept>{2, L"isConnected"},
      SyncMethod<double(, ) noexcept>{3, L"compareDocumentPosition"},
      SyncMethod<std::string() noexcept>{4, L"getTextContent"},
      SyncMethod<std::optional<std::vector<double>>(, bool) noexcept>{5, L"getBoundingClientRect"},
      SyncMethod<std::optional<std::vector<>>() noexcept>{6, L"getOffset"},
      SyncMethod<std::optional<std::vector<double>>() noexcept>{7, L"getScrollPosition"},
      SyncMethod<std::optional<std::vector<double>>() noexcept>{8, L"getScrollSize"},
      SyncMethod<std::optional<std::vector<double>>() noexcept>{9, L"getInnerSize"},
      SyncMethod<std::optional<std::vector<double>>() noexcept>{10, L"getBorderSize"},
      SyncMethod<std::string() noexcept>{11, L"getTagName"},
      SyncMethod<bool(, double) noexcept>{12, L"hasPointerCapture"},
      Method<void(, double) noexcept>{13, L"setPointerCapture"},
      Method<void(, double) noexcept>{14, L"releasePointerCapture"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, DOMSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getParentNode",
          "    REACT_SYNC_METHOD(getParentNode) std::optional<> getParentNode( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getParentNode) static std::optional<> getParentNode( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getChildNodes",
          "    REACT_SYNC_METHOD(getChildNodes) std::optional<std::vector<>> getChildNodes( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getChildNodes) static std::optional<std::vector<>> getChildNodes( shadowNode) noexcept { /* implementation */ }\n");
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
          "    REACT_SYNC_METHOD(getBoundingClientRect) std::optional<std::vector<double>> getBoundingClientRect( shadowNode, bool includeTransform) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBoundingClientRect) static std::optional<std::vector<double>> getBoundingClientRect( shadowNode, bool includeTransform) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getOffset",
          "    REACT_SYNC_METHOD(getOffset) std::optional<std::vector<>> getOffset( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getOffset) static std::optional<std::vector<>> getOffset( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getScrollPosition",
          "    REACT_SYNC_METHOD(getScrollPosition) std::optional<std::vector<double>> getScrollPosition( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollPosition) static std::optional<std::vector<double>> getScrollPosition( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getScrollSize",
          "    REACT_SYNC_METHOD(getScrollSize) std::optional<std::vector<double>> getScrollSize( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getScrollSize) static std::optional<std::vector<double>> getScrollSize( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "getInnerSize",
          "    REACT_SYNC_METHOD(getInnerSize) std::optional<std::vector<double>> getInnerSize( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getInnerSize) static std::optional<std::vector<double>> getInnerSize( shadowNode) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getBorderSize",
          "    REACT_SYNC_METHOD(getBorderSize) std::optional<std::vector<double>> getBorderSize( shadowNode) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBorderSize) static std::optional<std::vector<double>> getBorderSize( shadowNode) noexcept { /* implementation */ }\n");
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
  }
};

} // namespace Microsoft::ReactNativeSpecs
