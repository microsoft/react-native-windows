
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

struct ExceptionsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, React::JSValueArray, double) noexcept>{0, L"reportFatalException"},
      Method<void(std::string, React::JSValueArray, double) noexcept>{1, L"reportSoftException"},
      Method<void(React::JSValueObject) noexcept>{2, L"reportException"},
      Method<void(std::string, React::JSValueArray, double) noexcept>{3, L"updateExceptionMessage"},
      Method<void() noexcept>{4, L"dismissRedbox"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ExceptionsManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "reportFatalException",
          "    REACT_METHOD(reportFatalException) void reportFatalException(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportFatalException) static void reportFatalException(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "reportSoftException",
          "    REACT_METHOD(reportSoftException) void reportSoftException(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportSoftException) static void reportSoftException(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "reportException",
          "    REACT_METHOD(reportException) void reportException(React::JSValueObject && data) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportException) static void reportException(React::JSValueObject && data) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "updateExceptionMessage",
          "    REACT_METHOD(updateExceptionMessage) void updateExceptionMessage(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(updateExceptionMessage) static void updateExceptionMessage(std::string message, React::JSValueArray && stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "dismissRedbox",
          "    REACT_METHOD(dismissRedbox) void dismissRedbox() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismissRedbox) static void dismissRedbox() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
