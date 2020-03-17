// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_MOTIFCPPTEST_H
#define MSO_MOTIFCPP_MOTIFCPPTEST_H

#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <functional>
#include <string>
#include <type_traits>
#include "comUtil/IUnknownShim.h"
#include "motifCpp/gTestAdapter.h"
#include "oacr/oacr.h"

using WCHAR = wchar_t;

#define GTEST_ASSERT_AT_(file, line, expression, on_failure)    \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_                                 \
  if (const ::testing::AssertionResult gtest_ar = (expression)) \
    ;                                                           \
  else                                                          \
    on_failure(file, line, gtest_ar.failure_message())

#define GTEST_FATAL_FAILURE_AT_(file, line, message) \
  return GTEST_MESSAGE_AT_(file, line, message, ::testing::TestPartResult::kFatalFailure)

namespace TestAssert {

inline std::string FormatMsg(char const *format, ...) noexcept {
  std::string result;
  va_list vlist;
  va_start(vlist, format);
  auto size = std::vsnprintf(nullptr, 0, format, vlist);
  result.append(size + 1, '\0');
  std::vsnprintf(&result[0], size + 1, format, vlist);
  result.resize(size);
  va_end(vlist);
  return result;
}

inline std::string FormatCustomMsg(int line, char const *message = "") {
  return FormatMsg(
      "    Line: %d%s%s",
      line,
      (message == nullptr || message[0] == '\0') ? "" : "\n",
      message == nullptr ? "" : message);
}

inline void FailInternalAt(char const *file, int line, char const *message, std::string &&errorMessage) {
  GTEST_FATAL_FAILURE_AT_(file, line, "") << errorMessage << FormatCustomMsg(line, message).c_str();
}

inline void FailAt(char const *file, int line, char const *message = "") {
  FailInternalAt(file, line, message, "Failed\n");
}

inline void IsTrueAt(char const *file, int line, bool condition, char const *exprStr, char const *message = "") {
  if (!condition) {
    FailInternalAt(
        file,
        line,
        message,
        FormatMsg(
            "Expected: [ %s ] is true\n"
            "  Actual: it is false\n",
            exprStr));
  }
}

template <class T, std::enable_if_t<!std::is_same_v<bool, T>, int> = 1>
inline void IsTrueAt(char const *file, int line, T condition, char const *exprStr, char const *message = "") {
  return IsTrueAt(file, line, !!condition, exprStr, message);
}

template <class TExpected, class TActual>
inline void AreEqualAt(
    char const *file,
    int line,
    TExpected const &expected,
    TActual const &actual,
    char const *expectedExpr,
    char const *actualExpr,
    char const *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::EqHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare(
          expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

template <
    class TExpected,
    class TActual,
    std::enable_if_t<
        std::is_same_v<TExpected, TActual> && !std::is_same_v<TExpected, WCHAR> && !std::is_same_v<TExpected, char>,
        int> = 1>
void AreEqualAt(
    char const *file,
    int line,
    TExpected const *expected,
    TActual const *actual,
    char const *expectedExpr,
    char const *actualExpr,
    char const *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::EqHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare(
          expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

inline void AreEqualAt(
    char const *file,
    int line,
    WCHAR const *expected,
    WCHAR const *actual,
    char const *expectedExpr,
    char const *actualExpr,
    char const *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::CmpHelperSTREQ(expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

inline void AreEqualAt(
    char const *file,
    int line,
    char const *expected,
    char const *actual,
    char const *expectedExpr,
    char const *actualExpr,
    char const *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::CmpHelperSTREQ(expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

inline void AreEqualAt(
    char const *file,
    int line,
    WCHAR *expected,
    WCHAR const *actual,
    char const *expectedExpr,
    char const *actualExpr,
    char const *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::CmpHelperSTREQ(expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

inline void AreEqualAt(
    char const *file,
    int line,
    char *expected,
    const char *actual,
    char const *expectedExpr,
    char const *actualExpr,
    const char *message = "") {
  GTEST_ASSERT_AT_(
      file,
      line,
      ::testing::internal::CmpHelperSTREQ(expectedExpr, actualExpr, expected, actual),
      GTEST_FATAL_FAILURE_AT_)
      << FormatCustomMsg(line, message);
}

// Code used for all the C++ exceptions
constexpr uint32_t EXCEPTION_CPLUSPLUS = static_cast<uint32_t>(0xE06D7363);

inline uint32_t FilterCrashExceptions(uint32_t exceptionCode) noexcept {
  if ((exceptionCode == EXCEPTION_BREAKPOINT) // allow exceptions to get to the debugger
      || (exceptionCode == EXCEPTION_SINGLE_STEP) // allow exceptions to get to the debugger
      || (exceptionCode == EXCEPTION_GUARD_PAGE) // allow to crash on memory page access violation
      || (exceptionCode == EXCEPTION_STACK_OVERFLOW)) // allow to crash on stack overflow
  {
    return EXCEPTION_CONTINUE_SEARCH;
  }

  if (exceptionCode == EXCEPTION_CPLUSPLUS) // log C++ exception and pass it through
  {
    FailAt(__FILE__, __LINE__, "Test function did not crash, but exception is thrown!");
    return EXCEPTION_CONTINUE_SEARCH;
  }

  return EXCEPTION_EXECUTE_HANDLER;
}

template <class TLambda>
inline bool ExpectCrashCore(TLambda const &lambda) {
  __try {
    lambda();
    return false;
  } __except (FilterCrashExceptions(GetExceptionCode())) {
    return true;
  }
}

template <class TLambda>
inline void
ExpectCrashAt(char const *file, int line, TLambda const &lambda, char const *exprStr, char const *message = "") {
  if (!ExpectCrashCore(lambda)) {
    FailInternalAt(
        file,
        line,
        message,
        FormatMsg(
            "Expected: [ %s ] must crash\n"
            "  Actual: it does not crash\n",
            exprStr));
  }
}

struct TerminateHandlerRestorer {
  ~TerminateHandlerRestorer() noexcept {
    std::set_terminate(Handler);
  }

  std::terminate_handler Handler;
};

#pragma warning(push)
#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
template <class TLambda>
inline bool ExpectTerminateCore(TLambda const &lambda) {
  static jmp_buf buf;

  // Set a terminate handler and save the previous terminate handler to be restored in the end of function.
  TerminateHandlerRestorer terminateRestore = {std::set_terminate([]() { longjmp(buf, 1); })};

  // setjmp originally returns 0, and when longjmp is called it returns 1.
  if (!setjmp(buf)) {
    lambda();
    return false; // must not be executed if fn() caused termination and the longjmp is executed.
  } else {
    return true; // executed if longjmp is executed in the terminate handler.
  }
}
#pragma warning(pop)

template <class TLambda>
inline void
ExpectTerminateAt(char const *file, int line, TLambda const &lambda, char const *exprStr, char const *message = "") {
  if (!ExpectTerminateCore(lambda)) {
    FailInternalAt(
        file,
        line,
        message,
        FormatMsg(
            "Expected: [ %s ] must terminate\n"
            "  Actual: it does not terminate\n",
            exprStr));
  }
}

template <class TException, class TLambda>
inline void ExpectExceptionAt(
    char const *file,
    int line,
    TLambda const &lambda,
    char const *exceptionStr,
    char const *exprStr,
    char const *message = "") {
  char const *actualIssue = "";
  bool isFailed = false;
  bool isExpectedExceptionCaught = false;

  try {
    lambda();
  } catch (TException const &) {
    isExpectedExceptionCaught = true;
  } catch (...) {
    isFailed = true;
    actualIssue = "it throws a different type";
  }

  if (!isExpectedExceptionCaught && !isFailed) {
    isFailed = true;
    actualIssue = "it does not throw";
  }

  if (isFailed) {
    FailInternalAt(
        file,
        line,
        message,
        FormatMsg(
            "Expected: [ %s ] throws an exception of type %s\n"
            "  Actual: %s\n",
            exprStr,
            exceptionStr,
            actualIssue));
  }
}

template <class TLambda>
inline void
ExpectNoThrowAt(char const *file, int line, TLambda const &lambda, char const *exprStr, char const *message = "") {
  try {
    lambda();
  } catch (...) {
    FailInternalAt(
        file,
        line,
        message,
        FormatMsg(
            "Expected: [ %s ] does not throw an exception\n"
            "  Actual: it throws an exception\n",
            exprStr));
  }
}

// Asserts that the specified condition is true, if it is false the unit test will fail
inline void IsTrue(bool condition, _In_z_ const WCHAR *message = L"") {
  ASSERT_TRUE(condition) << message;
}

// Verify that a condition is true:
template <typename T, typename TEnable = typename std::enable_if<!std::is_same<bool, T>::value>::type>
inline void IsTrue(const T &condition, _In_z_ const WCHAR *message = L"") {
  return IsTrue(!!condition, message);
}

// Asserts that the specified condition is false, if it is true the unit test will fail
inline void IsFalse(bool condition, const WCHAR *message = L"") {
  ASSERT_FALSE(condition) << message;
}

template <typename T, typename TEnable = typename std::enable_if<!std::is_same<bool, T>::value>::type>
inline void IsFalse(const T &condition, _In_z_ const WCHAR *message = L"") {
  return IsFalse(!!condition, message);
}

template <typename ExpectedType, typename ActualType>
inline void
AreEqual(_In_ const ExpectedType &expected, _In_ const ActualType &actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_EQ(expected, actual) << wstrMessage.c_str();
}

template <
    typename ExpectedType,
    typename ActualType,
    typename TEnable = typename std::enable_if<
        std::is_same<ExpectedType, ActualType>::value && !std::is_same<ExpectedType, WCHAR>::value &&
        !std::is_same<ExpectedType, char>::value>::type>
void AreEqual(_In_ const ExpectedType *expected, _In_ const ActualType *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_EQ(expected, actual) << wstrMessage.c_str();
}

inline void AreEqual(_In_ const WCHAR *expected, _In_ const WCHAR *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STREQ(expected, actual) << wstrMessage.c_str();
}

inline void AreEqual(_In_ const char *expected, _In_ const char *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STREQ(expected, actual) << wstrMessage.c_str();
}

inline void AreEqual(_In_ WCHAR *expected, _In_ const WCHAR *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STREQ(expected, actual) << wstrMessage.c_str();
}

inline void AreEqual(_In_ char *expected, _In_ const char *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STREQ(expected, actual) << wstrMessage.c_str();
}

template <typename ExpectedType, typename ActualType>
inline void
AreNotEqual(_In_ const ExpectedType &expected, _In_ const ActualType &actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_NE(expected, actual) << wstrMessage.c_str();
}

template <
    typename ExpectedType,
    typename ActualType,
    typename TEnable = typename std::enable_if<
        std::is_same<ExpectedType, ActualType>::value && !std::is_same<ExpectedType, WCHAR>::value &&
        !std::is_same<ExpectedType, char>::value>::type>
void AreNotEqual(_In_ const ExpectedType *expected, _In_ const ActualType *actual, _In_z_ const WCHAR *message = L"") {
  AreNotEqual(*expected, *actual, message);
}

inline void AreNotEqual(_In_ const WCHAR *expected, _In_ const WCHAR *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STRNE(expected, actual) << wstrMessage.c_str();
}

inline void AreNotEqual(_In_ const char *expected, _In_ const char *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STRNE(expected, actual) << wstrMessage.c_str();
}

inline void AreNotEqual(_In_ WCHAR *expected, _In_ const WCHAR *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STRNE(expected, actual) << wstrMessage.c_str();
}

inline void AreNotEqual(_In_ char *expected, _In_ const char *actual, _In_z_ const WCHAR *message = L"") {
  std::wstring wstrMessage(message);
  ASSERT_STRNE(expected, actual) << wstrMessage.c_str();
}

inline void Comment(_In_ const WCHAR *message) {
  std::wcerr << L"[ INFO     ] " << message << std::endl;
}

inline void CommentEx(_In_z_ _Printf_format_string_ const WCHAR *format, va_list args) noexcept {
  fwprintf(stderr, L"[ INFO     ] ");
  vfwprintf(stderr, format, args);
  fwprintf(stderr, L"\n");
}

inline void CommentEx(_In_z_ _Printf_format_string_ const WCHAR *format, ...) noexcept {
  va_list args;
  va_start(args, format);
  fwprintf(stderr, L"[ INFO     ] ");
  vfwprintf(stderr, format, args);
  va_end(args);
  fwprintf(stderr, L"\n");
}

inline void Pass(_In_z_ const WCHAR *message = L"") noexcept {
  ASSERT_TRUE(true) << message;
}

inline void Fail(_In_z_ const WCHAR *message = L"") {
  ASSERT_TRUE(false) << message;
}

template <typename ValueType>
inline void IsNull(const ValueType &ptr, _In_z_ const WCHAR *message = L"") {
  AreEqual(ptr, nullptr, message);
}

template <typename ValueType>
inline void IsNotNull(const ValueType &ptr, _In_z_ const WCHAR *message = L"") {
  AreNotEqual(ptr, nullptr, message);
}

template <typename ExceptionType>
inline void ExpectException(const std::function<void()> &statement, const WCHAR *message = L"") {
  EXPECT_THROW(statement(), ExceptionType) << message;
}

template <typename ExceptionType>
inline void ExpectException(
    const std::function<void()> &statement,
    const std::function<void()> &onException,
    const WCHAR *message = L"") {
  EXPECT_THROW(statement(), ExceptionType) << message;
}

inline void ExpectNoThrow(const std::function<void()> &statement, const WCHAR *message = L"") {
  EXPECT_NO_THROW(statement()) << message;
}

inline void HrSucceeded(HRESULT hr, _In_z_ const WCHAR *message = L"") {
  ASSERT_TRUE(SUCCEEDED(hr)) << message;
}

inline void HrFailed(HRESULT hr, _In_z_ const WCHAR *message = L"") {
  ASSERT_FALSE(SUCCEEDED(hr)) << message;
}

template <typename Fn>
inline void ExpectVEC(const Fn &fn, const WCHAR *message = L"") {
  if (!ExpectCrashCore(fn)) {
    Fail(message);
  }
}

} // namespace TestAssert

#endif // MSO_MOTIFCPP_MOTIFCPPTEST_H
