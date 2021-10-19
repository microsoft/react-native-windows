
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

REACT_STRUCT(StatusBarManagerIOSSpec_getHeight_callback_result)
struct StatusBarManagerIOSSpec_getHeight_callback_result {
    REACT_FIELD(height)
    double height;
};

REACT_STRUCT(StatusBarManagerIOSSpec_Constants)
struct StatusBarManagerIOSSpec_Constants {
    REACT_FIELD(HEIGHT)
    double HEIGHT;
    REACT_FIELD(DEFAULT_BACKGROUND_COLOR)
    std::optional<double> DEFAULT_BACKGROUND_COLOR;
};

struct StatusBarManagerIOSSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<StatusBarManagerIOSSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(Callback<StatusBarManagerIOSSpec_getHeight_callback_result>) noexcept>{0, L"getHeight"},
      Method<void(bool) noexcept>{1, L"setNetworkActivityIndicatorVisible"},
      Method<void(std::string) noexcept>{2, L"addListener"},
      Method<void(double) noexcept>{3, L"removeListeners"},
      Method<void(std::optional<std::string>, bool) noexcept>{4, L"setStyle"},
      Method<void(bool, std::string) noexcept>{5, L"setHidden"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, StatusBarManagerIOSSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, StatusBarManagerIOSSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "StatusBarManagerIOSSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) StatusBarManagerIOSSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static StatusBarManagerIOSSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "getHeight",
          "    REACT_METHOD(getHeight) void getHeight(std::function<void(StatusBarManagerIOSSpec_getHeight_callback_result const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getHeight) static void getHeight(std::function<void(StatusBarManagerIOSSpec_getHeight_callback_result const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setNetworkActivityIndicatorVisible",
          "    REACT_METHOD(setNetworkActivityIndicatorVisible) void setNetworkActivityIndicatorVisible(bool visible) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setNetworkActivityIndicatorVisible) static void setNetworkActivityIndicatorVisible(bool visible) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventType) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventType) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "setStyle",
          "    REACT_METHOD(setStyle) void setStyle(std::optional<std::string> statusBarStyle, bool animated) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setStyle) static void setStyle(std::optional<std::string> statusBarStyle, bool animated) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "setHidden",
          "    REACT_METHOD(setHidden) void setHidden(bool hidden, std::string withAnimation) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setHidden) static void setHidden(bool hidden, std::string withAnimation) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
