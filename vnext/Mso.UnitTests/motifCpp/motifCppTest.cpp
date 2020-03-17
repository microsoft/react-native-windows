// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "crash/verifyElseCrash.h"
#include "motifCpp/testCheck.h"

// Uncomment to see errors
//#define MOTIF_TEST_SHOW_ERRORS

TEST_CLASS (MotifCppTest) {
#ifdef MOTIF_TEST_SHOW_ERRORS

  TEST_METHOD(GTestFailCalled) {
    FAIL();
  }

  TEST_METHOD(GTestFailCalledWithMessage) {
    FAIL() << "Test must fail";
  }

  TEST_METHOD(GTestFailCalledWithMessageArgs) {
    FAIL() << TestAssert::FormatMsg("Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(GTestAssertTrueFailed) {
    ASSERT_TRUE(std::string("Foo") == "Bar");
  }

  TEST_METHOD(GTestAssertTrueFailedWithMessage) {
    ASSERT_TRUE(std::string("Foo") == "Bar") << "Test must fail";
  }

  TEST_METHOD(GTestAssertTrueFailedWithMessageArgs) {
    ASSERT_TRUE(std::string("Foo") == "Bar") << TestAssert::FormatMsg("Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(GTestAssertEQFailed) {
    ASSERT_EQ(std::string("Foo"), "Bar");
  }

  TEST_METHOD(GTestAssertEQFailedWithMessage) {
    ASSERT_EQ(std::string("Foo"), "Bar") << "Test must fail";
  }

  TEST_METHOD(GTestAssertEQFailedWithMessageArgs) {
    ASSERT_EQ(std::string("Foo"), "Bar") << TestAssert::FormatMsg("Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckFailCalled) {
    TestCheckFail();
  }

  TEST_METHOD(TestCheckFailCalledWithMessage) {
    TestCheckFail("Test must fail");
  }

  TEST_METHOD(TestCheckFailCalledWithMessageArgs) {
    TestCheckFail("Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckFailed) {
    TestCheck(std::string("Foo") == "Bar");
  }

  TEST_METHOD(TestCheckFailedWithMessage) {
    TestCheck(std::string("Foo") == "Bar", "Test must fail");
  }

  TEST_METHOD(TestCheckFailedWithMessageArgs) {
    TestCheck(std::string("Foo") == "Bar", "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckEqualFailed) {
    TestCheckEqual(std::string("Foo"), "Bar");
  }

  TEST_METHOD(TestCheckEqualFailedWithMessage) {
    TestCheckEqual(std::string("Foo"), "Bar", "Test must fail");
  }

  TEST_METHOD(TestCheckEqualFailedWithMessageArgs) {
    TestCheckEqual(std::string("Foo"), "Bar", "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckCrashFailed) {
    TestCheckCrash(VerifyElseCrashSz(true, "No crash"));
  }

  TEST_METHOD(TestCheckCrashFailedWithMessage) {
    TestCheckCrash(VerifyElseCrashSz(true, "No crash"), "Test must fail");
  }

  TEST_METHOD(TestCheckCrashFailedWithMessageArgs) {
    TestCheckCrash(VerifyElseCrashSz(true, "No crash"), "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckCrashSucceeded) {
    TestCheckCrash(VerifyElseCrashSz(false, "Must crash"));
  }

  TEST_METHOD(TestCheckCrashSucceededWithMessage) {
    TestCheckCrash(VerifyElseCrashSz(false, "Must crash"), "Test must succeed");
  }

  TEST_METHOD(TestCheckCrashSucceededWithMessageArgs) {
    TestCheckCrash(VerifyElseCrashSz(false, "Must crash"), "Test %s %s %d", "must", "succeed", 2);
  }

  TEST_METHOD(TestCheckTerminateFailed) {
    TestCheckTerminate(2 + 2);
  }

  TEST_METHOD(TestCheckTerminateFailedWithMessage) {
    TestCheckTerminate(2 + 2, "Test must fail");
  }

  TEST_METHOD(TestCheckTerminateFailedWithMessageArgs) {
    TestCheckTerminate(2 + 2, "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckTerminateSucceeded) {
    TestCheckTerminate(std::terminate());
  }

  TEST_METHOD(TestCheckTerminateSucceededWithMessage) {
    TestCheckTerminate(std::terminate(), "Test must succeed");
  }

  TEST_METHOD(TestCheckTerminateSucceededWithMessageArgs) {
    TestCheckTerminate(std::terminate(), "Test %s %s %d", "must", "succeed", 2);
  }

  TEST_METHOD(TestCheckExceptionFailed) {
    TestCheckException(std::exception, 2 + 2);
  }

  TEST_METHOD(TestCheckExceptionFailedWithMessage) {
    TestCheckException(std::exception, 2 + 2, "Test must fail");
  }

  TEST_METHOD(TestCheckExceptionFailedWithMessageArgs) {
    TestCheckException(std::exception, 2 + 2, "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckExceptionSucceeded) {
    TestCheckException(std::exception, throw std::exception());
  }

  TEST_METHOD(TestCheckExceptionSucceededWithMessage) {
    TestCheckException(std::exception, throw std::exception(), "Test must succeed");
  }

  TEST_METHOD(TestCheckExceptionSucceededWithMessageArgs) {
    TestCheckException(std::exception, throw std::exception(), "Test %s %s %d", "must", "succeed", 2);
  }

  TEST_METHOD(TestCheckNoThrowFailed) {
    TestCheckNoThrow(throw std::exception());
  }

  TEST_METHOD(TestCheckNoThrowFailedWithMessage) {
    TestCheckNoThrow(throw std::exception(), "Test must fail");
  }

  TEST_METHOD(TestCheckNoThrowFailedWithMessageArgs) {
    TestCheckNoThrow(throw std::exception(), "Test %s %s %d", "must", "fail", 2);
  }

  TEST_METHOD(TestCheckNoThrowSucceeded) {
    TestCheckNoThrow(2 + 2);
  }

  TEST_METHOD(TestCheckNoThrowSucceededWithMessage) {
    TestCheckNoThrow(2 + 2, "Test must succeed");
  }

  TEST_METHOD(TestCheckNoThrowSucceededWithMessageArgs) {
    TestCheckNoThrow(2 + 2, "Test %s %s %d", "must", "succeed", 2);
  }
#endif
};
