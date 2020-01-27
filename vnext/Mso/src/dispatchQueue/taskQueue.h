// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <vector>
#include "dispatchQueue/dispatchQueue.h"
#include "threadMutex.h"

namespace Mso {

//! ReadBuffer is used by task queue to dequeue items.
//! It moves read index instead of resizing buffer.
struct TaskReadBuffer {
  TaskReadBuffer() = default;

  // Prohibit copy and move
  TaskReadBuffer(TaskReadBuffer const &other) = delete;
  TaskReadBuffer &operator=(TaskReadBuffer const &other) = delete;

  bool TryDequeue(DispatchTask &task) noexcept;
  void SwapBuffer(std::vector<DispatchTask> &buffer) noexcept;
  size_t Size() const noexcept;
  bool IsEmpty() const noexcept;

 private:
  std::vector<DispatchTask> m_buffer;
  size_t m_index{0};
};

//! Simple queue that uses two buffers: one for pushing items and another for popping them.
//! Items are always enqueued under lock.
//!
//! Internally we have two vectors: one to enqueue items (write) and another to dequeue items (read).
//! When the read queue is empty we swap them.
struct TaskQueue {
  TaskQueue(Mso::WeakPtr<IUnknown> &&weakOwnerPtr) noexcept;

  ~TaskQueue() noexcept;

  // Prohibit copy and move
  TaskQueue(TaskQueue const &other) = delete;
  TaskQueue &operator=(TaskQueue const &other) = delete;

  void Enqueue(DispatchTask &&task) noexcept;
  bool TryDequeue(DispatchTask &task) noexcept;
  bool DequeueAll(/*out*/ std::vector<DispatchTask> &tasks) noexcept;
  size_t Size() const noexcept;
  bool IsEmpty() const noexcept;

 private:
  std::vector<DispatchTask> m_writeBuffer; // To enqueue items.
  TaskReadBuffer m_readBuffer; // To dequeue items.
  Mso::WeakPtr<IUnknown> m_weakOwnerPtr;
  Mso::CntPtr<IUnknown> m_strongOwnerPtr; // Keep strong reference to the owner when queu is not empty;
};

} // namespace Mso
