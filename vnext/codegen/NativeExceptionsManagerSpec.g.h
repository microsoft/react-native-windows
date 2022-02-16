
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

REACT_STRUCT(ExceptionsManagerSpec_StackFrame)
struct ExceptionsManagerSpec_StackFrame {
    REACT_FIELD(column)
    std::optional<double> column;
    REACT_FIELD(file)
    std::optional<std::string> file;
    REACT_FIELD(lineNumber)
    std::optional<double> lineNumber;
    REACT_FIELD(methodName)
    std::string methodName;
    REACT_FIELD(collapse)
    std::optional<bool> collapse;
};

REACT_STRUCT(ExceptionsManagerSpec_ExceptionData)
struct ExceptionsManagerSpec_ExceptionData {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(originalMessage)
    std::optional<std::string> originalMessage;
    REACT_FIELD(name)
    std::optional<std::string> name;
    REACT_FIELD(componentStack)
    std::optional<std::string> componentStack;
    REACT_FIELD(stack)
    std::vector<ExceptionsManagerSpec_StackFrame> stack;
    REACT_FIELD(id)
    double id;
    REACT_FIELD(isFatal)
    bool isFatal;
    REACT_FIELD(extraData)
    std::optional<::React::JSValue> extraData;
};

struct ExceptionsManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::string, std::vector<ExceptionsManagerSpec_StackFrame>, double) noexcept>{0, L"reportFatalException"},
      Method<void(std::string, std::vector<ExceptionsManagerSpec_StackFrame>, double) noexcept>{1, L"reportSoftException"},
      Method<void(ExceptionsManagerSpec_ExceptionData) noexcept>{2, L"reportException"},
      Method<void(std::string, std::vector<ExceptionsManagerSpec_StackFrame>, double) noexcept>{3, L"updateExceptionMessage"},
      Method<void() noexcept>{4, L"dismissRedbox"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ExceptionsManagerSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "reportFatalException",
          "    REACT_METHOD(reportFatalException) void reportFatalException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportFatalException) static void reportFatalException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "reportSoftException",
          "    REACT_METHOD(reportSoftException) void reportSoftException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportSoftException) static void reportSoftException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "reportException",
          "    REACT_METHOD(reportException) void reportException(ExceptionsManagerSpec_ExceptionData && data) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(reportException) static void reportException(ExceptionsManagerSpec_ExceptionData && data) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "updateExceptionMessage",
          "    REACT_METHOD(updateExceptionMessage) void updateExceptionMessage(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(updateExceptionMessage) static void updateExceptionMessage(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "dismissRedbox",
          "    REACT_METHOD(dismissRedbox) void dismissRedbox() noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(dismissRedbox) static void dismissRedbox() noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
