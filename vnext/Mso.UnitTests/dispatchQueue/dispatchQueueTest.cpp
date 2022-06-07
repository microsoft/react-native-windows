// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "motifCpp/testCheck.h"

namespace Mso {
extern void Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(bool enable) noexcept;
extern void Test_ThreadPoolSchedulerWin_WaitForThreadPoolWorkCompletion() noexcept;
} // namespace Mso

namespace DispatchQueueTests {

TEST_CLASS (DispatchQueueTest) {
  TEST_METHOD(DispatchQueue_Shutdown) {
    // Check that there is no dead lock if queue is released outside of task.
    std::mutex mutex;
    std::atomic<bool> taskStarted{false};
    std::condition_variable whenTaskStarted;
    int callCount = 0;
    {
      Mso::DispatchQueue queue = Mso::DispatchQueue::MakeSerialQueue();
      queue.Post([&]() {
        ++callCount;
        taskStarted = true;
        whenTaskStarted.notify_all();
      });
      std::unique_lock lock(mutex);
      whenTaskStarted.wait(lock, [&]() { return taskStarted.load(); });
    }
    TestCheck(callCount == 1);
  }

  TEST_METHOD(DispatchQueue_ShutdownFromTask) {
    // Check that there is no dead lock if queue is released form a task.
    // Use shared_ptr for mutex because the test completes before the DispatchQueue task.
    std::shared_ptr<std::mutex> mutex = std::make_shared<std::mutex>();
    std::atomic<bool> taskStarted{false};
    std::condition_variable whenTaskStarted;
    std::atomic<bool> queueReleased{false};
    std::condition_variable whenQueueReleased;
    int callCount = 0;
    Mso::Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(true);
    {
      Mso::DispatchQueue queue = Mso::DispatchQueue::MakeSerialQueue();
      queue.Post([&, mutex]() {
        ++callCount;
        std::unique_lock lock(*mutex);
        taskStarted = true;
        whenTaskStarted.notify_all();
        // Wait until the queue is released outside of task.
        whenQueueReleased.wait(lock, [&]() { return queueReleased.load(); });
      });
      std::unique_lock lock(*mutex);
      whenTaskStarted.wait(lock, [&]() { return taskStarted.load(); });
    }
    {
      std::unique_lock lock(*mutex);
      queueReleased = true;
      whenQueueReleased.notify_all();
    }
    TestCheck(callCount == 1);
    Mso::Test_ThreadPoolSchedulerWin_WaitForThreadPoolWorkCompletion();
    Mso::Test_ThreadPoolSchedulerWin_EnableThreadPoolWorkTracking(false);
  }
};

} // namespace DispatchQueueTests
