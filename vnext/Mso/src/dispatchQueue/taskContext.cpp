// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "taskContext.h"

namespace Mso {

TaskContext::TaskContext(
    IDispatchQueueService *queue,
    std::optional<std::chrono::steady_clock::time_point> endTime) noexcept
    : m_prevContext{tls_context}, m_queue{queue}, m_endTime{endTime} {
  tls_context = this;
}

TaskContext::~TaskContext() noexcept {
  tls_context = m_prevContext;
}

void TaskContext::Defer(DispatchTask &&task) noexcept {
  m_deferQueue.push_back(std::move(task));
}

DispatchTask TaskContext::TakeNextDeferredTask() noexcept {
  return m_readIndex < m_deferQueue.size() ? std::move(m_deferQueue[m_readIndex++]) : DispatchTask{};
}

/*static*/ TaskContext *TaskContext::CurrentContext() noexcept {
  return tls_context;
}

/*static*/ IDispatchQueueService *TaskContext::CurrentQueue() noexcept {
  if (auto context = CurrentContext()) {
    return context->m_queue;
  }

  return nullptr;
}

} // namespace Mso
