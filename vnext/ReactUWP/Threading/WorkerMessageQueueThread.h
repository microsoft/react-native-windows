// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace react {
namespace uwp {

// Executes func on random worker thread provided by Windows ThreadPool.
// Serial execution is guaranteed.
// Must be destroyed from a UI or JavaScript thread. Destroying from a
// background thread can cause deadlocks! Same applies for quitSynchronous().
class WorkerMessageQueueThread : public facebook::react::MessageQueueThread {
 public:
  WorkerMessageQueueThread();
  virtual ~WorkerMessageQueueThread();

  virtual void runOnQueue(std::function<void()> &&func);
  virtual void runOnQueueSync(std::function<void()> &&func);
  virtual void quitSynchronous();

 private:
  struct Impl;
  std::unique_ptr<Impl> m_pimpl;
};

} // namespace uwp
} // namespace react
