// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "queueService.h"
#include "taskBatch.h"
#include "taskContext.h"

namespace Mso {

//=============================================================================
// QueueService implementation.
//=============================================================================

QueueService::QueueService(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept
    : m_scheduler{std::move(scheduler)} {
  m_scheduler->IntializeScheduler(this);
}

QueueService::~QueueService() noexcept {
  AwaitTermination();
}

void QueueService::Post(DispatchTask &&task) noexcept {
  VerifyElseCrashSz(task, "The task is empty");

  bool isShutdown = false;
  bool shouldSchedule = false;

  {
    std::lock_guard lock{m_mutex};
    auto it = m_taskBatches.find(std::this_thread::get_id());
    if (it != m_taskBatches.end()) {
      it->second->AddTask(std::move(task));
    } else {
      isShutdown = m_shutdownAction.has_value();
      if (!isShutdown) {
        m_queue.Enqueue(std::move(task));
        shouldSchedule = (m_suspendCounter == 0);
      }
    }
  }

  if (shouldSchedule) {
    m_scheduler->Post();
  } else if (isShutdown) {
    CancelTask(std::move(task));
  }
}

bool QueueService::ShouldYield(TaskYieldReason *yieldReason) noexcept {
  auto setReason = [&](TaskYieldReason reason) noexcept { return yieldReason ? *yieldReason = reason : reason, true; };
  std::lock_guard lock{m_mutex};
  return (m_shutdownAction.has_value() && setReason(TaskYieldReason::QueueShutdown)) ||
      (m_suspendCounter > 0 && setReason(TaskYieldReason::QueueSuspended));
}

bool QueueService::IsCurrentQueue() noexcept {
  return TaskContext::CurrentQueue() == this;
}

bool QueueService::IsSerial() noexcept {
  return m_scheduler->IsSerial();
}

bool QueueService::HasThreadAccess() noexcept {
  return m_scheduler->HasThreadAccess();
}

void QueueService::InvokeElsePost(DispatchTask &&task) noexcept {
  if (HasThreadAccess()) {
    if (TaskContext::CurrentQueue() == this) {
      task.Get()->Invoke();
      task = nullptr;
    } else {
      InvokeTask(std::move(task), std::nullopt);
    }
  } else {
    Post(std::move(task));
  }
}

void QueueService::DeferElsePost(DispatchTask &&task) noexcept {
  if (TaskContext::CurrentQueue() == this) {
    TaskContext::CurrentContext()->Defer(std::move(task));
  } else {
    Post(std::move(task));
  }
}

void QueueService::BeginTaskBatching() noexcept {
  auto taskBatch{Mso::Make<TaskBatch>()};
  std::lock_guard lock{m_mutex};
  auto result = m_taskBatches.try_emplace(std::this_thread::get_id(), std::move(taskBatch));
  if (result.second) {
    taskBatch->SetEnclosingBatch(std::move(result.first->second));
    result.first->second = std::move(taskBatch);
  }
}

DispatchTask QueueService::EndTaskBatching() noexcept {
  Mso::CntPtr<TaskBatch> taskBatch;
  std::lock_guard lock{m_mutex};
  auto it = m_taskBatches.find(std::this_thread::get_id());
  if (it != m_taskBatches.end()) {
    taskBatch = std::move(it->second);
    if (auto enclosingBatch = taskBatch->TakeEnclosingBatch()) {
      it->second = std::move(enclosingBatch);
    } else {
      m_taskBatches.erase(it);
    }
  } else {
    taskBatch = Mso::Make<TaskBatch>();
  }

  return {std::move(taskBatch)};
}

bool QueueService::HasTaskBatching() noexcept {
  std::lock_guard lock{m_mutex};
  return m_taskBatches.find(std::this_thread::get_id()) != m_taskBatches.end();
}

bool QueueService::TryLockQueueLocalValue(SwapDispatchLocalValueCallback swapLocalValue, void **tlsValue) noexcept {
  return TrySwapLocalValue(swapLocalValue, tlsValue, LocalValueSwapAction::Lock);
}

void QueueService::UnlockLocalValue(void **tlsValue) noexcept {
  TrySwapLocalValue(nullptr, tlsValue, LocalValueSwapAction::Unlock);
}

bool QueueService::TrySwapLocalValue(
    SwapDispatchLocalValueCallback swapLocalValue,
    void **tlsValue,
    LocalValueSwapAction action) noexcept {
  std::lock_guard lock{m_mutex};

  // TLS value has a different address in each thread, but all TLS values must have the same
  // layout between threads. We use address delta between the provided TLS value and
  // our internal localValueAnchor. It must be the same for a TLS value between threads.
  static thread_local uint8_t localValueAnchor{0};
  ptrdiff_t key = static_cast<uint8_t *>(static_cast<void *>(tlsValue)) - &localValueAnchor;
  auto [it, added] = m_localValues.try_emplace(key, swapLocalValue);
  return it->second.TrySwapLocalValue(tlsValue, action);
}

void QueueService::Suspend() noexcept {
  std::lock_guard lock{m_mutex};
  ++m_suspendCounter;
}

void QueueService::Resume() noexcept {
  size_t postCount{0};

  {
    std::lock_guard lock{m_mutex};
    VerifyElseCrashSz(m_suspendCounter > 0, "m_suspendCounter must not be negative");

    if (--m_suspendCounter == 0) {
      postCount = m_queue.Size();
    }
  }

  for (size_t i = 0; i < postCount; ++i) {
    m_scheduler->Post();
  }
}

void QueueService::Shutdown(PendingTaskAction pendingTaskAction) noexcept {
  std::vector<DispatchTask> tasksToCancel;

  {
    std::lock_guard lock{m_mutex};
    m_shutdownAction = pendingTaskAction;
    if (pendingTaskAction == PendingTaskAction::Cancel) {
      m_queue.DequeueAll(/*out*/ tasksToCancel);
    }
  }

  for (auto &task : tasksToCancel) {
    CancelTask(std::move(task));
  }

  m_scheduler->Shutdown();
}

void QueueService::AwaitTermination() noexcept {
  Shutdown(PendingTaskAction::Complete);
  m_scheduler->AwaitTermination();
}

bool QueueService::HasTasks() noexcept {
  std::lock_guard lock{m_mutex};
  return m_suspendCounter == 0 && !m_queue.IsEmpty();
}

bool QueueService::TryDequeTask(/*out*/ DispatchTask &task) noexcept {
  std::lock_guard lock{m_mutex};
  return m_suspendCounter == 0 && m_queue.TryDequeue(/*out*/ task);
}

void QueueService::InvokeTask(
    DispatchTask &&task,
    std::optional<std::chrono::steady_clock::time_point> endTime) noexcept {
  TaskContext context{this, endTime};
  DispatchTask taskToInvoke{std::move(task)};
  taskToInvoke.Get()->Invoke(); // Call Get()->Invoke instead of operator() to flatten call stack

  while (taskToInvoke = context.TakeNextDeferredTask()) {
    taskToInvoke.Get()->Invoke();
  }
}

void QueueService::CancelTask(DispatchTask &&task) noexcept {
  DispatchTask taskToCancel{std::move(task)};
  if (auto cancellation = query_cast<ICancellationListener *>(taskToCancel.Get())) {
    cancellation->OnCancel();
  }
}

//=============================================================================
// LocalValueEntry implementation.
//=============================================================================

QueueLocalValueEntry::QueueLocalValueEntry(SwapDispatchLocalValueCallback swapLocalValue) noexcept
    : m_swapLocalValue{swapLocalValue} {}

QueueLocalValueEntry::~QueueLocalValueEntry() noexcept {
  m_swapLocalValue(&m_data, nullptr);
}

bool QueueLocalValueEntry::TrySwapLocalValue(void **tlsValue, LocalValueSwapAction action) noexcept {
  if (action == LocalValueSwapAction::Lock && !m_isLocked) {
    m_isLocked = true;
  } else if (action == LocalValueSwapAction::Unlock && m_isLocked) {
    m_isLocked = false;
  } else {
    return false;
  }

  m_swapLocalValue(&m_data, tlsValue);
  return true;
}

//=============================================================================
// IDispatchQueueStatic implementation.
//=============================================================================

/*static*/ IDispatchQueueStatic *IDispatchQueueStatic::Instance() noexcept {
  return DispatchQueueStatic::Instance();
}

//=============================================================================
// DispatchQueueStatic implementation.
//=============================================================================

/*static*/ DispatchQueueStatic *DispatchQueueStatic::Instance() noexcept {
  static std::unique_ptr<DispatchQueueStatic> instance{new DispatchQueueStatic()};
  return instance.get();
}

DispatchQueue DispatchQueueStatic::CurrentQueue() noexcept {
  return Mso::CntPtr{TaskContext::CurrentQueue()};
}

DispatchQueue const &DispatchQueueStatic::ConcurrentQueue() noexcept {
  static auto concurrentQueue{Mso::Make<QueueService, IDispatchQueueService>(MakeThreadPoolScheduler(0))};
  return *static_cast<DispatchQueue *>(static_cast<void *>(&concurrentQueue));
}

DispatchQueue DispatchQueueStatic::MakeSerialQueue() noexcept {
  return Mso::Make<QueueService, IDispatchQueueService>(MakeThreadPoolScheduler(/*maxThreads:*/ 1));
}

DispatchQueue DispatchQueueStatic::MakeLooperQueue() noexcept {
  return Mso::Make<QueueService, IDispatchQueueService>(MakeLooperScheduler());
}

DispatchQueue DispatchQueueStatic::MakeConcurrentQueue(uint32_t maxThreads) noexcept {
  return Mso::Make<QueueService, IDispatchQueueService>(MakeThreadPoolScheduler(maxThreads));
}

DispatchQueue DispatchQueueStatic::MakeCustomQueue(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept {
  return Mso::Make<QueueService, IDispatchQueueService>(std::move(scheduler));
}

} // namespace Mso
