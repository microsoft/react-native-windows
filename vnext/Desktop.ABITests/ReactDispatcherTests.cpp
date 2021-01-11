// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>

#include <chrono>
#include <future>

using namespace std::chrono_literals;
using namespace winrt;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

TEST_CLASS (ReactDispatcherTests) {
 public:
  TEST_METHOD(HasThreadAccess_FalseForTestThread) {
    IReactDispatcher dispatcher = ReactDispatcherHelper::CreateSerialDispatcher();
    TestCheck(!dispatcher.HasThreadAccess());
  }
  TEST_METHOD(Post_CallbackGetsExecuted) {
    IReactDispatcher dispatcher = ReactDispatcherHelper::CreateSerialDispatcher();
    std::promise<void> callbackExecutionPromise;
    auto callbackExecuted = callbackExecutionPromise.get_future();

    dispatcher.Post([&callbackExecutionPromise] { callbackExecutionPromise.set_value(); });

    TestCheckEqual(std::future_status::ready, callbackExecuted.wait_for(500ms));
  }
};

} // namespace ABITests
