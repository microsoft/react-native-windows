// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/details/executor.h"

namespace Mso::Executors {

Executor::Executor(DispatchQueue const &queue) noexcept : m_queue{queue} {}

Executor::Executor(DispatchQueue &&queue) noexcept : m_queue{std::move(queue)} {}

void Executor::Post(DispatchTask &&task) noexcept {
  if (auto queue = m_queue) {
    queue.Post(std::move(task));
  } else {
    DispatchQueue::ConcurrentQueue().Post(std::move(task));
  }
}

void Concurrent::Post(DispatchTask &&task) noexcept {
  DispatchQueue::ConcurrentQueue().Post(std::move(task));
}

void Inline::Post(DispatchTask &&task) noexcept {
  task.Get()->Invoke();
  task = nullptr;
}

} // namespace Mso::Executors
