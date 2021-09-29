
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

REACT_STRUCT(JSDevSupportSpec_Constants)
struct JSDevSupportSpec_Constants {
    REACT_FIELD(ERROR_CODE_EXCEPTION)
    double ERROR_CODE_EXCEPTION;
    REACT_FIELD(ERROR_CODE_VIEW_NOT_FOUND)
    double ERROR_CODE_VIEW_NOT_FOUND;
};

struct JSDevSupportSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<JSDevSupportSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"onSuccess"},
      Method<void(double, std::string) noexcept>{1, L"onFailure"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, JSDevSupportSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, JSDevSupportSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "JSDevSupportSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) JSDevSupportSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static JSDevSupportSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "onSuccess",
          "    REACT_METHOD(onSuccess) void onSuccess(std::string data) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onSuccess) static void onSuccess(std::string data) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "onFailure",
          "    REACT_METHOD(onFailure) void onFailure(double errorCode, std::string error) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(onFailure) static void onFailure(double errorCode, std::string error) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
