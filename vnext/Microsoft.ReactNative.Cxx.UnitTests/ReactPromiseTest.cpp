// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <ReactPromise.h>

using namespace winrt::Microsoft::ReactNative;

namespace ReactNativeTests {

TEST_CLASS (ReactPromiseTest) {
  TEST_METHOD(Test_ReactPromise_void_resolve) {
    bool isSucceeded = false;
    bool isFailed = false;
    ReactPromise<void> promise(
        [&isSucceeded]() noexcept { isSucceeded = true; },
        [&isFailed](ReactError const &) noexcept { isFailed = true; });
    promise.Resolve();

    TestCheck(isSucceeded);
    TestCheck(!isFailed);
  }

  TEST_METHOD(Test_ReactPromise_void_reject) {
    bool isSucceeded = false;
    bool isFailed = false;
    ReactPromise<void> promise(
        [&isSucceeded]() noexcept { isSucceeded = true; },
        [&isFailed](ReactError const &) noexcept { isFailed = true; });
    promise.Reject("Failed");

    TestCheck(!isSucceeded);
    TestCheck(isFailed);
  }

  TEST_METHOD(Test_ReactPromise_int_resolve) {
    int resultValue = 0;
    bool isFailed = false;
    ReactPromise<int> promise(
        [&resultValue](int value) noexcept { resultValue = value; },
        [&isFailed](ReactError const &) noexcept { isFailed = true; });
    promise.Resolve(42);

    TestCheckEqual(resultValue, 42);
    TestCheck(!isFailed);
  }

  TEST_METHOD(Test_ReactPromise_int_reject) {
    int resultValue = 0;
    bool isFailed = false;
    ReactPromise<int> promise(
        [&resultValue](int value) noexcept { resultValue = value; },
        [&isFailed](ReactError const &) noexcept { isFailed = true; });
    promise.Reject("Failed");

    TestCheckEqual(resultValue, 0);
    TestCheck(isFailed);
  }
};

} // namespace ReactNativeTests
