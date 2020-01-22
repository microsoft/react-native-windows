// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_MOTIFCPP_TESTCHECK_H
#define MSO_MOTIFCPP_TESTCHECK_H

//=============================================================================
// Test macros for better error reporting.
// They allow to report the line number and the failing expression.
// It allows to write more compact code because we do not need to add comment
// for every TestAssert.
//
// All macros have a form with suffix "L" that accept line number as a parameter.
// It is done to enable creation of custom check macros that internally
// use the macros we provide.
//=============================================================================

#include <csetjmp>
#include <csignal>
#include "motifCpp/assert_motifApi.h"

//=============================================================================
// A helper macro to provide current line number as a wide char string.
//=============================================================================
#ifndef MSO_TO_STR
#define MSO_INTERNAL_TO_STR(value) #value
#define MSO_TO_STR(value) MSO_INTERNAL_TO_STR(value)
#endif

#ifndef MSO_WIDE_STR
#define MSO_INTERNAL_WIDE_STR(str) L##str
#define MSO_WIDE_STR(str) MSO_INTERNAL_WIDE_STR(str)
#endif

#define MSO_LINE_STR MSO_TO_STR(__LINE__)
#define MSO_LINE_WIDE_STR MSO_WIDE_STR(MSO_LINE_STR)

//=============================================================================
// TestCheckFail fails the test unconditionally.
//=============================================================================
#define TestCheckFailL(message, line) TestAssert::Fail(MSO_WIDE_STR("Line: " line " " message))
#define TestCheckFail(message) TestCheckFailL(message, MSO_LINE_STR)

//=============================================================================
// TestCheck checks if provided expression evaluates to true.
// If check fails then it reports the line number and the failed expression.
//=============================================================================
#define TestCheckL(expr, line) TestAssert::IsTrue(expr, MSO_WIDE_STR("Line: " line " [ " MSO_TO_STR(expr) " ]"))
#define TestCheck(expr) TestCheckL(expr, MSO_LINE_STR)

//=============================================================================
// TestCheckEqual checks if two provided values are equal.
// If check fails then it reports the line number and the failed expression.
// In addition the TestAssert::AreEqual reports expected and actual values.
//=============================================================================
#define TestCheckEqualL(expected, actual, line) \
  TestAssert::AreEqual(                         \
      expected, actual, MSO_WIDE_STR("Line: " line " [ " MSO_TO_STR(expected) " == " MSO_TO_STR(actual) " ]"))
#define TestCheckEqual(expected, actual) TestCheckEqualL(expected, actual, MSO_LINE_STR)

//=============================================================================
// TestCheckIgnore ignores the provided expression.
// It can be used to avoid compilation errors related to unused variables.
// It evaluates the expression, but ignores its result.
//=============================================================================
#define TestCheckIgnore(expr) (void)expr

//=============================================================================
// TestCheckCrash expects that the provided expression causes a crash.
//=============================================================================
//	Mso::IgnoreAllAsserts ignore;
#define TestCheckCrashL(expr, line) \
  TestAssert::ExpectVEC(            \
      [&]() {                       \
        OACR_POSSIBLE_THROW;        \
        expr;                       \
      },                            \
      MSO_WIDE_STR("Line: " line " Must crash: [ " MSO_TO_STR(expr) " ]"))
#define TestCheckCrash(expr) TestCheckCrashL(expr, MSO_LINE_STR)

//=============================================================================
// TestCheckTerminate expects that the provided expression causes process termination
// with a call to std::terminate().
// It is useful to check when a noexcept function throws, or std::terminate is called directly.
// The implementation uses setjmp and longjmp which stops the termination sequence.
// In case if TestCheckTerminate succeeds we may end up with leaked memory because
// the call stack is not unwinded correctly.
// You should disable memory leak detection in tests that use TestCheckTerminate.
//=============================================================================
#define TestCheckTerminateL(expr, line) \
  TestAssert::ExpectTerminate([&]() { expr; }, MSO_WIDE_STR("Line: " line " Must terminate: [ " MSO_TO_STR(expr) " ]"))
#define TestCheckTerminate(expr) TestCheckTerminateL(expr, MSO_LINE_STR)

//=============================================================================
// TestCheckException expects that the provided expression throws an exception.
//=============================================================================
#define TestCheckExceptionL(ex, expr, line) \
  TestAssert::ExpectException<ex>(          \
      [&]() { expr; }, MSO_WIDE_STR("Line: " line " Must throw: " MSO_TO_STR(ex) " [ " MSO_TO_STR(expr) " ]"))
#define TestCheckException(ex, expr) TestCheckExceptionL(ex, expr, MSO_LINE_STR)

//=============================================================================
// TestCheckNoThrow expects that the provided expression does not throw an exception.
//=============================================================================
#define TestCheckNoThrowL(expr, line) \
  TestAssert::ExpectNoThrow([&]() { expr; }, MSO_WIDE_STR("Line: " line " Must not throw: [ " MSO_TO_STR(expr) " ]"))
#define TestCheckNoThrow(expr) TestCheckNoThrowL(expr, MSO_LINE_STR)

//=============================================================================
// TestCheckAssert checks for the code to produce assert with specified tag.
//=============================================================================
#define TestCheckShipAssert(tag, expr) Statement(Mso::ExpectShipAssert expectAssert(tag); expr;);

//=============================================================================
// A macro to disable memory leak detection in unit tests where we expect crash
// or terminate. These tests cannot do proper stack unwinding and thus are
// going to leak memory. In order to use this macro you need to turn on
// memory leak detection per test method instead of the default per-class.
// It is done by adding the following member to the test class:
// MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;
//=============================================================================
#define TEST_DISABLE_MEMORY_LEAK_DETECTION() \
  //StopTrackingMemoryAllocations(); \
	//auto restartTrackingMemoryAllocations = Mso::TCleanup::Make([&]() noexcept \
	//{ \
	//	StartTrackingMemoryAllocations(); \
	//});

//=============================================================================
// Helper functions to implement TestCheckTerminate.
//=============================================================================
namespace TestAssert {

struct TerminateHandlerRestorer {
  ~TerminateHandlerRestorer() noexcept {
    std::set_terminate(Handler);
  }

  std::terminate_handler Handler;
};

#pragma warning(push)
#pragma warning(disable : 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
template <class Fn>
inline bool ExpectTerminateCore(const Fn &fn) {
  static jmp_buf buf;

  // Set a terminate handler and save the previous terminate handler to be restored in the end of function.
  TerminateHandlerRestorer terminateRestore = {std::set_terminate([]() { longjmp(buf, 1); })};

  // setjmp originally returns 0, and when longjmp is called it returns 1.
  if (!setjmp(buf)) {
    fn();
    return false; // must not be executed if fn() caused termination and the longjmp is executed.
  } else {
    return true; // executed if longjmp is executed in the terminate handler.
  }
}
#pragma warning(pop)

template <class Fn>
inline void ExpectTerminate(const Fn &fn, const WCHAR *message = L"") {
  if (!ExpectTerminateCore(fn)) {
    Fail(message == nullptr || message[0] == L'\0' ? L"Test function did not terminate!" : message);
  }
}

} // namespace TestAssert

#endif // MSO_MOTIFCPP_TESTCHECK_H
