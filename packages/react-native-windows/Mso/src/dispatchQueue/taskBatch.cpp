// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "taskBatch.h"

namespace Mso {

//=============================================================================
// TaskBatch implementation.
//=============================================================================

void TaskBatch::AddTask(DispatchTask &&task) noexcept {
  m_tasks.push_back(std::move(task));
}

void TaskBatch::SetEnclosingBatch(Mso::CntPtr<TaskBatch> &&batch) noexcept {
  m_enclosingBatch = std::move(batch);
}

Mso::CntPtr<TaskBatch> TaskBatch::TakeEnclosingBatch() noexcept {
  return std::move(m_enclosingBatch);
}

void TaskBatch::Invoke() noexcept {
  for (auto &task : m_tasks) {
    task.Get()->Invoke();
    task = nullptr;
  }
}

void TaskBatch::OnCancel() noexcept {
  for (auto &task : m_tasks) {
    if (auto listener = query_cast<ICancellationListener *>(task.Get())) {
      listener->OnCancel();
    }

    task = nullptr;
  }
}

} // namespace Mso
