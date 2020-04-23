
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

struct BugReportingSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"startReportAProblemFlow"},
      Method<void(JSValueObject, JSValueObject) noexcept>{1, L"setExtraData"},
      Method<void(std::string) noexcept>{2, L"setCategoryID"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, BugReportingSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "startReportAProblemFlow",
          "    REACT_METHOD(startReportAProblemFlow) void startReportAProblemFlow() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(startReportAProblemFlow) static void startReportAProblemFlow() noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "setExtraData",
          "    REACT_METHOD(setExtraData) void setExtraData(React::JSValueObject && extraData, React::JSValueObject && extraFiles) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setExtraData) static void setExtraData(React::JSValueObject && extraData, React::JSValueObject && extraFiles) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "setCategoryID",
          "    REACT_METHOD(setCategoryID) void setCategoryID(std::string categoryID) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(setCategoryID) static void setCategoryID(std::string categoryID) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
