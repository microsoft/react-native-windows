// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Shared/BatchingMessageQueueThread.h>
#include <thread>

namespace react::uwp {

// Executes the function on the provided UI Dispatcher
struct BatchingQueueThread final : facebook::react::BatchingMessageQueueThread {
  BatchingQueueThread(std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept;
  ~BatchingQueueThread() noexcept override;

  BatchingQueueThread() = delete;
  BatchingQueueThread(const BatchingQueueThread &other) = delete;

 public: // facebook::react::MessageQueueThread
  void runOnQueue(std::function<void()> &&func) noexcept override;
  void runOnQueueSync(std::function<void()> &&func) noexcept override;
  void quitSynchronous() noexcept override;

 public: // facebook::react::BatchingMessageQueueThread
  void onBatchComplete() noexcept override;

 private:
  void EnsureQueue() noexcept;
  void ThreadCheck() noexcept;
  void PostBatch() noexcept;

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;

  using WorkItemQueue = std::vector<std::function<void()>>;
  std::shared_ptr<WorkItemQueue> m_taskQueue;
  std::mutex m_mutex;

#if DEBUG
  std::thread::id m_expectedThreadId{};
#endif
};

} // namespace react::uwp
