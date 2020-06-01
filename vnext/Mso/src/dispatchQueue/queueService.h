// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <map>
#include <thread>
#include "eventWaitHandle/eventWaitHandle.h"
#include "object/refCountedObject.h"
#include "taskQueue.h"

namespace Mso {

// Forward declarations
struct QueueLocalValueEntry;
struct QueueService;
struct TaskBatch;

enum class LocalValueSwapAction {
  Lock,
  Unlock,
};

// A base class for serial dispatch queues
struct QueueService : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueService, IDispatchQueue> {
  QueueService(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept;
  ~QueueService() noexcept override;

  QueueService(QueueService const &other) = delete;
  QueueService &operator=(QueueService const &other) = delete;

 public: // IDispatchQueueService
  void Post(DispatchTask &&task) noexcept override;
  bool ShouldYield(TaskYieldReason *yieldReason) noexcept override;
  bool IsCurrentQueue() noexcept override;
  bool IsSerial() noexcept override;
  bool HasThreadAccess() noexcept override;
  void InvokeElsePost(DispatchTask &&task) noexcept override;
  void DeferElsePost(DispatchTask &&task) noexcept override;
  void BeginTaskBatching() noexcept override;
  DispatchTask EndTaskBatching() noexcept override;
  bool HasTaskBatching() noexcept override;
  bool TryLockQueueLocalValue(SwapDispatchLocalValueCallback swapLocalValue, void **tlsValue) noexcept override;
  void UnlockLocalValue(void **tlsValue) noexcept override;
  void Suspend() noexcept override;
  void Resume() noexcept override;
  void Shutdown(PendingTaskAction pendingTaskAction) noexcept override;
  void AwaitTermination() noexcept override;
  bool HasTasks() noexcept override;
  bool TryDequeTask(/*out*/ DispatchTask &task) noexcept override;
  void InvokeTask(DispatchTask &&task, std::optional<std::chrono::steady_clock::time_point> endTime) noexcept override;
  void CancelTask(DispatchTask &&task) noexcept override;

 private:
  bool TrySwapLocalValue(
      SwapDispatchLocalValueCallback swapLocalValue,
      void **tlsValue,
      LocalValueSwapAction action) noexcept;

 private:
  const Mso::CntPtr<IDispatchQueueScheduler> m_scheduler;
  ThreadMutex m_mutex;
  TaskQueue m_queue{static_cast<IDispatchQueue *>(this)};
  std::optional<PendingTaskAction> m_shutdownAction;
  int32_t m_suspendCounter{0};
  std::map<std::thread::id, Mso::CntPtr<TaskBatch>> m_taskBatches;
  std::map<ptrdiff_t, QueueLocalValueEntry> m_localValues;
};

// Stores a queue local value
struct QueueLocalValueEntry {
  QueueLocalValueEntry(SwapDispatchLocalValueCallback swapLocalValue) noexcept;
  ~QueueLocalValueEntry() noexcept;
  bool TrySwapLocalValue(void **tlsValue, LocalValueSwapAction action) noexcept;

 private:
  const SwapDispatchLocalValueCallback m_swapLocalValue;
  void *m_data{nullptr};
  bool m_isLocked{false};
};

struct DispatchQueueStatic : Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, IDispatchQueueStatic> {
  static DispatchQueueStatic *Instance() noexcept;
  static Mso::CntPtr<IDispatchQueueScheduler> MakeLooperScheduler() noexcept;
  static Mso::CntPtr<IDispatchQueueScheduler> MakeThreadPoolScheduler(uint32_t maxThreads) noexcept;

 public: // IDispatchQueueStatic
  DispatchQueue CurrentQueue() noexcept override;
  DispatchQueue const &ConcurrentQueue() noexcept override;
  DispatchQueue MakeSerialQueue() noexcept override;
  DispatchQueue MakeLooperQueue() noexcept override;
  DispatchQueue GetCurrentUIThreadQueue() noexcept override;
  DispatchQueue MakeConcurrentQueue(uint32_t maxThreads) noexcept override;
  DispatchQueue MakeCustomQueue(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept override;
};

} // namespace Mso
