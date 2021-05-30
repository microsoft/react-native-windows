// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <vector>
#include "dispatchQueue/dispatchQueue.h"
#include "object/unknownObject.h"

namespace Mso {

struct TaskBatch : UnknownObject<QueryCastHidden<IVoidFunctor>, ICancellationListener> {
  void AddTask(DispatchTask &&task) noexcept;
  void SetEnclosingBatch(Mso::CntPtr<TaskBatch> &&batch) noexcept;
  Mso::CntPtr<TaskBatch> TakeEnclosingBatch() noexcept;

 public: // IVoidFunctor
  void Invoke() noexcept override;

 public: // ICancellationListener
  void OnCancel() noexcept override;

 private:
  std::vector<DispatchTask> m_tasks;
  Mso::CntPtr<TaskBatch> m_enclosingBatch;
};

} // namespace Mso
