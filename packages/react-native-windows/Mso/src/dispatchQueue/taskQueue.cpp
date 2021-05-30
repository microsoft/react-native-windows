// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "taskQueue.h"

namespace Mso {

//=============================================================================
// TaskReadBuffer implementation.
//=============================================================================

bool TaskReadBuffer::TryDequeue(DispatchTask &item) noexcept {
  if (m_index < m_buffer.size()) {
    item = std::move(m_buffer[m_index]);
    ++m_index;
    return true;
  }

  return false;
}

void TaskReadBuffer::SwapBuffer(std::vector<DispatchTask> &buffer) noexcept {
  // Before swapping erase deleted items in the beginning of the buffer.
  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_index);
  m_index = 0;
  m_buffer.swap(buffer);
}

size_t TaskReadBuffer::Size() const noexcept {
  return m_buffer.size() - m_index;
}

bool TaskReadBuffer::IsEmpty() const noexcept {
  return m_index == m_buffer.size();
}

//=============================================================================
// TaskQueue implementation.
//=============================================================================

TaskQueue::TaskQueue(Mso::WeakPtr<IUnknown> &&weakOwnerPtr) noexcept : m_weakOwnerPtr{std::move(weakOwnerPtr)} {}

TaskQueue::~TaskQueue() noexcept {
  VerifyElseCrashSz(IsEmpty(), "Queue must be empty before destruction.");
}

void TaskQueue::Enqueue(DispatchTask &&task) noexcept {
  bool queueWasEmpty{IsEmpty()};
  m_writeBuffer.push_back(std::move(task));
  if (queueWasEmpty) {
    m_strongOwnerPtr = m_weakOwnerPtr.GetStrongPtr();
  }
}

bool TaskQueue::TryDequeue(/*out*/ DispatchTask &task) noexcept {
  if (m_readBuffer.IsEmpty() && !m_writeBuffer.empty()) {
    m_readBuffer.SwapBuffer(m_writeBuffer);
  }

  bool result = m_readBuffer.TryDequeue(/*out*/ task);
  if (result && IsEmpty()) {
    m_strongOwnerPtr = nullptr;
  }

  return result;
}

bool TaskQueue::DequeueAll(/*out*/ std::vector<DispatchTask> &tasks) noexcept {
  if (IsEmpty()) {
    return false;
  }

  tasks.reserve(tasks.size() + m_readBuffer.Size() + m_writeBuffer.size());

  if (!m_readBuffer.IsEmpty()) {
    std::vector<DispatchTask> readBuffer;
    m_readBuffer.SwapBuffer(readBuffer);
    tasks.insert(tasks.end(), std::make_move_iterator(readBuffer.begin()), std::make_move_iterator(readBuffer.end()));
  }

  if (!m_writeBuffer.empty()) {
    tasks.insert(
        tasks.end(), std::make_move_iterator(m_writeBuffer.begin()), std::make_move_iterator(m_writeBuffer.end()));
    m_writeBuffer.clear();
  }

  m_strongOwnerPtr = nullptr;

  return true;
}

size_t TaskQueue::Size() const noexcept {
  return m_readBuffer.Size() + m_writeBuffer.size();
}

bool TaskQueue::IsEmpty() const noexcept {
  return m_readBuffer.IsEmpty() && m_writeBuffer.empty();
}

} // namespace Mso
