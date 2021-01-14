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
// All macros have a form with suffix "At" that accepts file and line number
// as parameters. It is done to enable creation of custom check macros that
// internally use these macros.
//=============================================================================

#include "motifCpp/assert_motifApi.h"

//=============================================================================
// TestCheckFail fails the test unconditionally.
//=============================================================================
#define TestCheckFailAt(file, line, ...) TestAssert::FailAt(file, line, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckFail(...) TestCheckFailAt(__FILE__, __LINE__, __VA_ARGS__)

//=============================================================================
// TestCheck checks if provided expression evaluates to true.
// If check fails then it reports the line number and the failed expression.
//
// Note that we convert expression to string before we pass it to an
// Internal macro. This is for better message in case if expression is a macro.
// It will be shown as macro usage rather than macro being expanded.
//=============================================================================
#define TestCheckAtInternal(file, line, expr, exprStr, ...) \
  TestAssert::IsTrueAt(file, line, !!(expr), exprStr, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckAt(file, line, expr, ...) TestCheckAtInternal(file, line, expr, #expr, __VA_ARGS__)
#define TestCheck(expr, ...) TestCheckAtInternal(__FILE__, __LINE__, expr, #expr, __VA_ARGS__)

//=============================================================================
// TestCheckEqual checks if two provided values are equal.
// If check fails then it reports the line number and the failed expression.
// In addition the TestAssert::AreEqual reports expected and actual values.
//=============================================================================
#define TestCheckEqualAtInternal(file, line, expected, actual, expectedStr, actualStr, ...) \
  TestAssert::AreEqualAt(                                                                   \
      file, line, expected, actual, expectedStr, actualStr, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckEqualAt(file, line, expected, actual, ...) \
  TestCheckEqualAtInternal(file, line, expected, actual, #expected, #actual, __VA_ARGS__)
#define TestCheckEqual(expected, actual, ...) \
  TestCheckEqualAtInternal(__FILE__, __LINE__, expected, actual, #expected, #actual, __VA_ARGS__)

//=============================================================================
// TestCheckIgnore ignores the provided expression.
// It can be used to avoid compilation errors related to unused variables.
// It evaluates the expression, but ignores its result.
//=============================================================================
#define TestCheckIgnore(expr) (void)expr

//=============================================================================
// TestCheckCrash expects that the provided expression causes a crash.
//=============================================================================
#define TestCheckCrashAtInternal(file, line, expr, exprStr, ...) \
  TestAssert::ExpectCrashAt(                                     \
      file, line, [&]() { expr; }, exprStr, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckCrashAt(file, line, expr, ...) TestCheckCrashAtInternal(file, line, expr, #expr, __VA_ARGS__)
#define TestCheckCrash(expr, ...) TestCheckCrashAtInternal(__FILE__, __LINE__, expr, #expr, __VA_ARGS__)

//=============================================================================
// TestCheckTerminate expects that the provided expression causes process termination
// with a call to std::terminate().
// It is useful to check when a noexcept function throws, or std::terminate is called directly.
// The implementation uses setjmp and longjmp which stops the termination sequence.
// In case if TestCheckTerminate succeeds we may end up with leaked memory because
// the call stack is not unwinded correctly.
// You should disable memory leak detection in tests that use TestCheckTerminate.
//=============================================================================
#define TestCheckTerminateAtInternal(file, line, expr, exprStr, ...) \
  TestAssert::ExpectTerminateAt(                                     \
      file, line, [&]() { expr; }, exprStr, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckTerminateAt(file, line, expr, ...) TestCheckTerminateAtInternal(file, line, expr, #expr, __VA_ARGS__)
#define TestCheckTerminate(expr, ...) TestCheckTerminateAtInternal(__FILE__, __LINE__, expr, #expr, __VA_ARGS__)

//=============================================================================
// TestCheckException expects that the provided expression throws an exception.
//=============================================================================
#define TestCheckExceptionAtInternal(file, line, ex, expr, exStr, exprStr, ...) \
  TestAssert::ExpectExceptionAt<ex>(                                            \
      file,                                                                     \
      line,                                                                     \
      [&]() {                                                                   \
        OACR_POSSIBLE_THROW;                                                    \
        expr;                                                                   \
      },                                                                        \
      exStr,                                                                    \
      exprStr,                                                                  \
      TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckExceptionAt(file, line, ex, expr, ...) \
  TestCheckExceptionAtInternal(file, line, ex, expr, #ex, #expr, __VA_ARGS__)
#define TestCheckException(ex, expr, ...) \
  TestCheckExceptionAtInternal(__FILE__, __LINE__, ex, expr, #ex, #expr, __VA_ARGS__)

//=============================================================================
// TestCheckNoThrow expects that the provided expression does not throw an exception.
//=============================================================================
#define TestCheckNoThrowAtInternal(file, line, expr, exprStr, ...) \
  TestAssert::ExpectNoThrowAt(                                     \
      file, line, [&]() { expr; }, exprStr, TestAssert::FormatMsg("" __VA_ARGS__).c_str())
#define TestCheckNoThrowAt(file, line, expr, ...) TestCheckNoThrowAtInternal(file, line, expr, #expr, __VA_ARGS__)
#define TestCheckNoThrow(expr, ...) TestCheckNoThrowAtInternal(__FILE__, __LINE__, expr, #expr, __VA_ARGS__)

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

#endif // MSO_MOTIFCPP_TESTCHECK_H
