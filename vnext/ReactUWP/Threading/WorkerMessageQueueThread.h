// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace react::uwp {

// Executes func on random worker thread provided by Windows ThreadPool.
// Serial execution is guaranteed.
// Must be destroyed from a UI or JavaScript thread. Destroying from a
// background thread can cause deadlocks! Same applies for quitSynchronous().
struct WorkerMessageQueueThread final : facebook::react::MessageQueueThread {
  WorkerMessageQueueThread() noexcept;
  ~WorkerMessageQueueThread() noexcept override;

 public: // facebook::react::MessageQueueThread
  void runOnQueue(std::function<void()> &&func) noexcept override;
  void runOnQueueSync(std::function<void()> &&func) noexcept override;
  void quitSynchronous() noexcept override;

 private:
  struct Impl;
  std::unique_ptr<Impl> m_pimpl;
};

} // namespace react::uwp
