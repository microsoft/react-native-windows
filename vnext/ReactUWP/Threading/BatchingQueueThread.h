// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/BatchingMessageQueueThread.h>
#include <thread>

namespace react::uwp {

// Executes the function on the provided UI Dispatcher
class BatchingQueueThread : public facebook::react::BatchingMessageQueueThread {
 public:
  BatchingQueueThread() = delete;
  BatchingQueueThread(const BatchingQueueThread &other) = delete;

  BatchingQueueThread(std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept;
  ~BatchingQueueThread() noexcept override;

  void runOnQueue(std::function<void()> &&func) noexcept override;
  void runOnQueueSync(std::function<void()> &&func) noexcept override;
  void quitSynchronous() noexcept override;

  void onBatchComplete() noexcept override;

 private:
  void ensureQueue() noexcept;
  void threadCheck() noexcept;

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;

  using WorkItemQueue = std::vector<std::function<void()>>;
  std::shared_ptr<WorkItemQueue> m_taskQueue;

#if DEBUG
  std::thread::id m_expectedThreadId{};
#endif
};

} // namespace react::uwp
