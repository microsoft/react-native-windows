// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "eventWaitHandle/eventWaitHandle.h"
#include "motifCpp/testCheck.h"
#include "winrt/Windows.Foundation.h"

using namespace winrt;
using namespace Windows::Foundation;
#include "winrt/Microsoft.UI.Dispatching.h"
using namespace Microsoft::UI::Dispatching;

namespace Mso {
extern void Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(bool enable) noexcept;
extern void Test_ThreadPoolSchedulerWin_WaitForThreadPoolWorkCompletion() noexcept;
} // namespace Mso

namespace DispatchQueueTests {

TEST_CLASS (DispatchQueueTest) {
  TEST_METHOD(DispatchQueue_SerialQueue_Shutdown) {
    WithSerialQueue(TestShutdown);
  }

  TEST_METHOD(DispatchQueue_UIQueue_Shutdown) {
    WithUIQueue(TestShutdown);
  }

  TEST_METHOD(DispatchQueue_UIQueue_ShutdownFromTask) {
    WithUIQueue(TestShutdownFromTask);
  }

  TEST_METHOD(DispatchQueue_SerialQueue_ShutdownFromTask) {
    WithSerialQueue(TestShutdownFromTask);
  }

  TEST_METHOD(DispatchQueue_UIQueue_ShutdownFromPreviousTask) {
    WithUIQueue(TestShutdownFromPreviousTask);
  }

  TEST_METHOD(DispatchQueue_SerialQueue_ShutdownFromPreviousTask) {
    WithSerialQueue(TestShutdownFromPreviousTask);
  }

 private:
  static void TestShutdown(Mso::DispatchQueue queue, Mso::VoidFunctor drainQueue) {
    // Check that there is no dead lock if queue is released outside of task.
    int callCount = 0;
    queue.Post([&]() { ++callCount; });
    drainQueue();

    queue = nullptr;
    TestCheck(callCount == 1);
  };

  static void TestShutdownFromTask(Mso::DispatchQueue queue, Mso::VoidFunctor drainQueue) {
    // Check that there is no dead lock if queue is released form a task.
    int callCount = 0;
    Mso::ManualResetEvent queueReleased;
    queue.Post([&]() {
      ++callCount;
      // Wait until the queue is released outside of task.
      // So, the last release of the queue will be done from the task.
      queueReleased.Wait();
    });

    queue = nullptr;
    queueReleased.Set();

    drainQueue();
    TestCheck(callCount == 1);
  }

  static void TestShutdownFromPreviousTask(Mso::DispatchQueue queue, Mso::VoidFunctor drainQueue) {
    // Check that there is no dead lock if queue is released form a task.
    int callCount = 0;
    Mso::ManualResetEvent taskStarted;
    Mso::ManualResetEvent queueReleased;
    queue.Post([&]() {
      taskStarted.Set();
      ++callCount;
      // Wait until the queue is released outside of task.
      // So, the last release of the queue will be done from the task.
      queueReleased.Wait();
    });

    queue.Post([&]() { ++callCount; });

    taskStarted.Wait();
    queue = nullptr;
    queueReleased.Set();

    drainQueue();
    TestCheck(callCount == 2);
  }

  static void WithUIQueue(Mso::Functor<void(Mso::DispatchQueue queue, Mso::VoidFunctor drainQueue)> test) {
    DispatcherQueueController queueController = DispatcherQueueController::CreateOnDedicatedThread();

    Mso::VoidFunctor drainQueue = [queueController]() noexcept {
      Mso::ManualResetEvent queueDrained;
      queueController.DispatcherQueue().TryEnqueue([queueDrained]() noexcept { queueDrained.Set(); });
      queueDrained.Wait();
    };

    Mso::DispatchQueue queue;
    queueController.DispatcherQueue().TryEnqueue(
        [&queue]() noexcept { queue = Mso::DispatchQueue::GetCurrentUIThreadQueue(); });
    drainQueue();

    test(std::move(queue), std::move(drainQueue));

    queueController.ShutdownQueueAsync().get();
  }

  static void WithSerialQueue(Mso::Functor<void(Mso::DispatchQueue queue, Mso::VoidFunctor drainQueue)> test) {
    Mso::Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(true);

    Mso::VoidFunctor drainQueue = []() noexcept { Mso::Test_ThreadPoolSchedulerWin_WaitForThreadPoolWorkCompletion(); };

    Mso::DispatchQueue queue = Mso::DispatchQueue::MakeSerialQueue();

    test(std::move(queue), drainQueue);

    drainQueue();
    Mso::Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(false);
  }
};

} // namespace DispatchQueueTests
