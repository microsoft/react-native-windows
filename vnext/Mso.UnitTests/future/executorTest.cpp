// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "dispatchQueue/dispatchQueue.h"
#include "future/future.h"
#include "future/futureWait.h"
#include "motifCpp/libletAwareMemLeakDetection.h"
#include "testCheck.h"
#include "testExecutor.h"

namespace FutureTests {

TEST_CLASS_EX (ExecutorTest, LibletAwareMemLeakDetection) {
  // MemoryLeakDetectionHook::TrackPerTest m_trackLeakPerTest;

  TEST_METHOD(ExecutorReference) {
    auto queue = MakeTestDispatchQueue();
    Mso::DispatchQueue &queueRef = queue;
    int value = 0;
    auto future = Mso::PostFuture(queueRef, [&]() noexcept { value = 5; });

    Mso::FutureWait(future);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(ExecutorConstReference) {
    auto queue = MakeTestDispatchQueue();
    Mso::DispatchQueue const &queueRef = queue;
    int value = 0;
    auto future = Mso::PostFuture(queueRef, [&]() noexcept { value = 5; });

    Mso::FutureWait(future);
    TestCheckEqual(5, value);
  }

  TEST_METHOD(ThrowingExecutorTestIntValue) {
    auto queue = MakeTestDispatchQueue();
    auto future = Mso::PostFuture(Mso::Executors::Executor::Throwing{queue}, []() -> int { return 5; })
                      .Then(queue, [&](const Mso::Maybe<int> &value) noexcept {
                        TestCheck(value.IsValue());
                        TestCheckEqual(5, value.GetValue());
                      });

    Mso::FutureWait(future);
  }

  TEST_METHOD(ThrowingExecutorTestIntException) {
    auto queue = MakeTestDispatchQueue();
    auto future = Mso::PostFuture(Mso::Executors::Executor::Throwing{queue}, []() -> int { throw std::exception(); })
                      .Then(queue, [&](const Mso::Maybe<int> &value) noexcept { TestCheck(value.IsError()); });

    Mso::FutureWait(future);
  }

  TEST_METHOD(ThrowingExecutorTestVoidValue) {
    auto queue = MakeTestDispatchQueue();
    auto future = Mso::PostFuture(
                      Mso::Executors::Executor::Throwing{queue},
                      []() {
                        // We return void
                      })
                      .Then(queue, [&](const Mso::Maybe<void> &value) noexcept { TestCheck(value.IsValue()); });

    Mso::FutureWait(future);
  }

  TEST_METHOD(ThrowingExecutorTestVoidException) {
    auto queue = MakeTestDispatchQueue();
    auto future = Mso::PostFuture(Mso::Executors::Executor::Throwing{queue}, []() -> void { throw std::exception(); })
                      .Then(queue, [&](const Mso::Maybe<void> &value) noexcept { TestCheck(value.IsError()); });

    Mso::FutureWait(future);
  }

  TEST_METHOD(DefaultExecutorPostFuture) {
    int value = 0;
    auto future = Mso::PostFuture([&]() noexcept { value = 5; });

    Mso::FutureWait(future);
    TestCheckEqual(5, value);
  }
};

} // namespace FutureTests
