
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

struct ExceptionsManagerSpec_StackFrame {
    std::optional<double> column;
    std::optional<std::string> file;
    std::optional<double> lineNumber;
    std::string methodName;
    std::optional<bool> collapse;
};

struct ExceptionsManagerSpec_ExceptionData {
    std::string message;
    std::optional<std::string> originalMessage;
    std::optional<std::string> name;
    std::optional<std::string> componentStack;
    std::vector<ExceptionsManagerSpec_StackFrame> stack;
    double id;
    bool isFatal;
    std::optional<::React::JSValue> extraData;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ExceptionsManagerSpec_StackFrame*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"column", &ExceptionsManagerSpec_StackFrame::column},
        {L"file", &ExceptionsManagerSpec_StackFrame::file},
        {L"lineNumber", &ExceptionsManagerSpec_StackFrame::lineNumber},
        {L"methodName", &ExceptionsManagerSpec_StackFrame::methodName},
        {L"collapse", &ExceptionsManagerSpec_StackFrame::collapse},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ExceptionsManagerSpec_ExceptionData*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"message", &ExceptionsManagerSpec_ExceptionData::message},
        {L"originalMessage", &ExceptionsManagerSpec_ExceptionData::originalMessage},
        {L"name", &ExceptionsManagerSpec_ExceptionData::name},
        {L"componentStack", &ExceptionsManagerSpec_ExceptionData::componentStack},
        {L"stack", &ExceptionsManagerSpec_ExceptionData::stack},
        {L"id", &ExceptionsManagerSpec_ExceptionData::id},
        {L"isFatal", &ExceptionsManagerSpec_ExceptionData::isFatal},
        {L"extraData", &ExceptionsManagerSpec_ExceptionData::extraData},
    };
    return fieldMap;
}

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
          "    REACT_METHOD(reportFatalException) void reportFatalException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(reportFatalException) static void reportFatalException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "reportSoftException",
          "    REACT_METHOD(reportSoftException) void reportSoftException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(reportSoftException) static void reportSoftException(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "reportException",
          "    REACT_METHOD(reportException) void reportException(ExceptionsManagerSpec_ExceptionData && data) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(reportException) static void reportException(ExceptionsManagerSpec_ExceptionData && data) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "updateExceptionMessage",
          "    REACT_METHOD(updateExceptionMessage) void updateExceptionMessage(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(updateExceptionMessage) static void updateExceptionMessage(std::string message, std::vector<ExceptionsManagerSpec_StackFrame> const & stack, double exceptionId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "dismissRedbox",
          "    REACT_METHOD(dismissRedbox) void dismissRedbox() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(dismissRedbox) static void dismissRedbox() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
