// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <vector>
#include "dispatchQueue/dispatchQueue.h"

namespace Mso {

//! Establishes a unique-per-thread task execution context.
//! Manages execution of deferred tasks.
struct TaskContext {
  TaskContext(IDispatchQueueService *queue, std::optional<std::chrono::steady_clock::time_point> endTime) noexcept;
  ~TaskContext() noexcept;

  void Defer(DispatchTask &&task) noexcept;
  DispatchTask TakeNextDeferredTask() noexcept;
  static TaskContext *CurrentContext() noexcept;
  static IDispatchQueueService *CurrentQueue() noexcept;

 private:
  inline static thread_local TaskContext *tls_context{nullptr};
  TaskContext *m_prevContext{nullptr};
  std::vector<DispatchTask> m_deferQueue;
  size_t m_readIndex{0};
  IDispatchQueueService *m_queue;
  std::optional<std::chrono::steady_clock::time_point> m_endTime;
};

} // namespace Mso
