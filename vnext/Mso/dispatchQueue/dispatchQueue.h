// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MSO_DISPATCHQUEUE_DISPATCHQUEUE_H
#define MSO_DISPATCHQUEUE_DISPATCHQUEUE_H

#include <optional>
#include <thread>
#include "functional/functor.h"
#include "object/unknownObject.h"
#include "span/span.h"
#include "typeTraits/tags.h"

namespace Mso {

//! Most of dispatch tasks implement just IVoidFunctor.
//! They can optionally implement ICancellationListener to observe cancellation.
//! They can implement any other interfaces if needed.
using DispatchTask = VoidFunctor;

// Forward declarations
struct DispatchLocalValueGuard;
struct DispatchQueue;
struct DispatchSuspendGuard;
struct DispatchTaskBatch;
template <typename TInvoke, typename TCancel>
struct DispatchTaskImpl;
template <typename TInvoke>
struct DispatchCleanupTaskImpl;
struct ICancellationListener;
struct IDispatchQueue;
struct IDispatchQueueScheduler;
struct IDispatchQueueService;
struct IDispatchQueueStatic;

//! A reason for a task being invoked to yield.
enum class TaskYieldReason {
  QueueShutdown,
  QueueSuspended,
  TimeExpired,
};

//! What to do with pending tasks on shutdown.
enum class PendingTaskAction {
  Complete,
  Cancel,
};

//! Callback type to handle queue local values
using SwapDispatchLocalValueCallback = void (*)(void **localValue, void **tlsValue) noexcept;

//! Gets a pointer to the state storage. This is a 'back-door' for advanced scenarios. Use with caution!
template <typename TObject>
auto GetRawState(TObject &&obj) noexcept;

//! Swaps queue local value with the TLS value.
//! It creates new value if localValue is null.
//! It destroys local value if tlsValue is null.
template <typename TValue>
void SwapDispatchLocalValue(void **localValue, void **tlsValue) noexcept;

template <typename TInvoke, typename TOnCancel>
Mso::VoidFunctor MakeDispatchTask(TInvoke &&invoke, TOnCancel &&onCancel) noexcept;

template <typename TInvoke>
Mso::VoidFunctor MakeDispatchCleanupTask(TInvoke &&invoke) noexcept;

//! RAII class to unlock the queue local value by swapping it back with TLS variable.
struct DispatchLocalValueGuard {
  //! Create new DispatchLocalValueGuard instance and swap TLS value with the queue local value.
  DispatchLocalValueGuard(IDispatchQueueService *queue, void **tlsValue) noexcept;

  //! Swap back the TLS value with the queue local value. It must run on the same thread as the constructor.
  ~DispatchLocalValueGuard() noexcept;

  //! Prohibit copy and move operations
  DispatchLocalValueGuard(DispatchLocalValueGuard const &other) = delete;
  DispatchLocalValueGuard &operator=(DispatchLocalValueGuard const &other) = delete;

  explicit operator bool() const noexcept;

 private:
  IDispatchQueueService *m_queue;
  void **m_tlsValue;
};

//! Serial or concurrent dispatch queue main API.
//! Use Post or InvokeElsePost to post tasks for invocation.
//! Use BeginTaskBatching to start tasks batching in current thread for the dispatch queue.
//! Use Suspend to temporary suspend task invocation.
//! Use Shutdown to finish task processing.
//! Use task's DispatchTaskContext to access dispatch queue local variables.
//! DispatchQueue is just a shared pointer to internal state and has size of a pointer. It is OK to copy and move.
struct DispatchQueue {
  //! Create new serial DispatchQueue.
  DispatchQueue() noexcept;

  //! Create empty DispatchQueue.
  DispatchQueue(std::nullptr_t) noexcept;

  //! Create new DispatchQueue with provided state.
  DispatchQueue(IDispatchQueueService *state, Mso::AttachTagType) noexcept;

  //! Create new DispatchQueue with provided state.
  DispatchQueue(Mso::CntPtr<IDispatchQueueService> &&state) noexcept;

  //! Return dispatch queue which currently running a task on this thread.
  //! If no task running, then it returns a queue with an empty state.
  static DispatchQueue CurrentQueue() noexcept;

  //! Get global concurrent queue which is a thin wrapper on top of platform specific thread pool. It is created on
  //! demand.
  static DispatchQueue const &ConcurrentQueue() noexcept;

  //! Create new serial DispatchQueue on top of platform specific thread pool.
  static DispatchQueue MakeSerialQueue() noexcept;

  //! Create new looper DispatchQueue on top of new std::thread. It owns the thread until shutdown.
  static DispatchQueue MakeLooperQueue() noexcept;

  //! Get a dispatch queue for the current UI thread. The result is null if the UI thread has no system UI thread
  //! dispatcher.
  static DispatchQueue GetCurrentUIThreadQueue() noexcept;

  //! Create a concurrent queue on top of platform specific thread pool that uses up to maxThreads threads.
  //! If maxThreads is zero, then it creates a concurrent queue that has a predefined limit on concurrently submitted
  //! work items.
  //! If maxThreads is one, then it calls MakeSerialQueue.
  //! If maxThreads is two or more, then it creates a concurrent queue with the maxThreads limit for concurrently
  //! running tasks.
  static DispatchQueue MakeConcurrentQueue(uint32_t maxThreads) noexcept;

  //! Create a dispatch queue on top of custom IDispatchQueueScheduler.
  //! The IDispatchQueueScheduler defines how the dispatch queue items are handled.
  static DispatchQueue MakeCustomQueue(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept;

  //! True if state is not empty.
  explicit operator bool() const noexcept;

  //! Post the task to the end of the queue for asynchronous invocation.
  void Post(DispatchTask &&task) const noexcept;

  //! Invoke the task immediately if the queue uses the current thread. Otherwise, post it.
  //! The immediate execution ignores the suspend or shutdown states.
  void InvokeElsePost(DispatchTask &&task) const noexcept;

  //! When called in the context of a task currently run by the queue then it is added to the task's deferred queue. The
  //! deferred tasks are invoked as a part of the current task, but after the current task unwinds the call stack.
  //! Otherwise, post it for the asynchronous invocation.
  void DeferElsePost(DispatchTask &&task) const noexcept;

  //! True if current task is invoked in context of this dispatch queue.
  bool IsCurrentQueue() const noexcept;

  //! True if tasks are invoked in a serial order by the queue.
  bool IsSerial() const noexcept;

  //! True if queue is running on current thread or associated with it.
  bool HasThreadAccess() const noexcept;

  //! Check if a long running task should yield.
  //! If provided yieldReason is not null, then it is assigned with a reason why the task is asked to yield.
  //! ShouldYield must not be checked at the start of task invocation because trivial implementation of ShouldYield
  //! always returns true and the long running task will never make any progress.
  bool ShouldYield(TaskYieldReason *yieldReason = nullptr) const noexcept;

  //! Start task batching on the current thread for this queue.
  //! All asynchronous task posted to the queue are going to be added to the returned DispatchTaskBatch
  //! until the DispatchTaskBatch is posted or canceled.
  DispatchTaskBatch StartTaskBatching() const noexcept;

  //! Tries to lock a queue local value (QLV) associated with the queue.
  //! If successful then the provided TLS variable is swapped with the corresponding QLV.
  //! The returned DispatchLocalValueGuard returns true when casted to bool in case of success. It can be used inside of
  //! an 'if' statement.
  template <typename TValue>
  DispatchLocalValueGuard LockLocalValue(TValue **tlsValue) const noexcept;

  //! Suspend asynchronous task invocation and return a guard that resumes it in its destructor.
  DispatchSuspendGuard Suspend() const noexcept;

  //! Shutdown the queue. All new asynchronous tasks are going to be canceled.
  //! Pending tasks are going to be completed or canceled depending on pendingTaskAction.
  void Shutdown(PendingTaskAction pendingTaskAction) const noexcept;

  //! Waits until all pending tasks are completed after shutdown.
  void AwaitTermination() const noexcept;

  //! True if the other dispatch queue has the same state pointer.
  [[nodiscard]] bool operator==(DispatchQueue const &other) const noexcept;

  //! True of the other dispatch queue has different state pointer.
  [[nodiscard]] bool operator!=(DispatchQueue const &other) const noexcept;

  //! A 'back-door' to get pointer to the state pointer. I.e. IDispatchQueue**.
  template <typename TObject>
  friend auto GetRawState(TObject &&obj) noexcept;

 private:
  Mso::CntPtr<IDispatchQueueService> m_state;
};

//! RAII class to resume dispatch queue task invocation after it was suspended.
//! It is created by DispatchQueue::Suspend() call.
//! DispatchSuspendGuard is just a shared pointer to internal state and has size of a pointer. It is OK to copy and
//! move. Note that when the class is copied, it calls IDispatchQueue::Suspend() to increment internal suspend counter.
struct DispatchSuspendGuard {
  //! Create empty DispatchSuspendGuard.
  DispatchSuspendGuard(std::nullptr_t = nullptr) noexcept;

  //! Create new DispatchSuspendGuard with provided state.
  DispatchSuspendGuard(Mso::CntPtr<IDispatchQueueService> const &state) noexcept;

  //! Copy calls IDispatchQueue::Suspend()
  DispatchSuspendGuard(DispatchSuspendGuard const &other) noexcept;
  DispatchSuspendGuard &operator=(DispatchSuspendGuard const &other) noexcept;

  // Move the DispatchSuspendGuard state.
  DispatchSuspendGuard(DispatchSuspendGuard &&other) noexcept = default;
  DispatchSuspendGuard &operator=(DispatchSuspendGuard &&other) noexcept = default;

  //! Call IDispatchQueue::Resume() if state is not empty.
  ~DispatchSuspendGuard() noexcept;

  //! True if state is not empty.
  explicit operator bool() const noexcept;

  //! A 'back-door' to get pointer to the state pointer. I.e. IDispatchQueueService**.
  template <typename TObject>
  friend auto GetRawState(TObject &&obj) noexcept;

 private:
  Mso::CntPtr<IDispatchQueueService> m_state;
};

//! RAII class to end task batching from current thread to the target dispatch queue.
//! It is created by DispatchQueue::StartTaskBatching() call.
//! If Post, InvokeElsePost, DeferElsePost, or Cancel are not called explicitly, then it calls Post for the task.
//! Note that all methods of this class must be called in the thread where it was created. It cannot be
//! copied and only can be moved.
struct DispatchTaskBatch {
  //! Create an invalid empty DispatchTaskBatch.
  DispatchTaskBatch(std::nullptr_t = nullptr) noexcept;

  //! Creates new DispatchTaskBatch in this thread targeting provided queue.
  DispatchTaskBatch(Mso::CntPtr<IDispatchQueueService> const &state) noexcept;

  // Prohibit DispatchTaskBatch copy.
  DispatchTaskBatch(DispatchTaskBatch const &other) = delete;
  DispatchTaskBatch &operator=(DispatchTaskBatch const &other) = delete;

  // Allow DispatchTaskBatch move.
  DispatchTaskBatch(DispatchTaskBatch &&other) = default;
  DispatchTaskBatch &operator=(DispatchTaskBatch &&other) = default;

  //! Post task batch if its state is not empty.
  ~DispatchTaskBatch() noexcept;

  //! True if state is not empty.
  explicit operator bool() const noexcept;

  //! Post the batch for invocation as a single task and clear the state.
  void Post() noexcept;

  //! Invoke or Post the batch as a single task and clear the state.
  void InvokeElsePost() noexcept;

  //! Defer or Post the batch as a single task and clear the state.
  void DeferElsePost() noexcept;

  //! Cancel the task batch and clear the state.
  void Cancel() noexcept;

  //! A 'back-door' to get pointer to the state pointer. I.e. IDispatchQueueService**.
  template <typename TObject>
  friend auto GetRawState(TObject &&obj) noexcept;

 private:
  Mso::CntPtr<IDispatchQueueService> m_state;
};

//! A dispatch queue task. The task can be either invoked or canceled.
MSO_GUID(ICancellationListener, "ec0f1ee4-b72d-4f50-8ba2-3131aeeb3663")
struct ICancellationListener : IUnknown {
  //! Cancel the task.
  virtual void OnCancel() noexcept = 0;
};

//! Simple dispatch queue interface that posts tasks for asynchronous invocation.
MSO_GUID(IDispatchQueue, "45b16d36-d4d7-4fe2-8af0-626bc39e1d3b")
struct IDispatchQueue : IUnknown {
  //! Add task to the end of asynchronous queue for invocation.
  virtual void Post(DispatchTask &&task) noexcept = 0;
};

//! Handles dispatch queue task execution on top of platform-specific scheduler.
//! A IDispatchQueueScheduler typically has a weak pointer to the IDispatchQueueService and
//! invokes tasks by calling IDispatchQueue's InvokeOneTask(), InvokeAllTasks(), or InvokeTasksFor() methods.
MSO_GUID(IDispatchQueueScheduler, "fd98f3c9-1343-4cbe-8758-a0322ffd253a")
struct IDispatchQueueScheduler : IUnknown {
  //! Initializes scheduler by providing the dispatch queue service.
  virtual void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept = 0;

  //! True if the scheduler does processing in a serial order.
  virtual bool IsSerial() noexcept = 0;

  //! True if queue is running on current thread or associated with it.
  virtual bool HasThreadAccess() noexcept = 0;

  //! Schedule handling of dispatch queue tasks.
  virtual void Post() noexcept = 0;

  //! Shutdown the scheduler. It initiates the shutdown process and cleans up resources.
  virtual void Shutdown() noexcept = 0;

  //! Returns after all scheduler resources are cleaned up.
  virtual void AwaitTermination() noexcept = 0;
};

//! Manages dispatch queue lifetime.
MSO_GUID(IDispatchQueueService, "2f97907f-6824-45e9-8884-c7ec0643b61f")
struct IDispatchQueueService : IUnknown {
  //! Add task to the end of asynchronous queue for invocation.
  virtual void Post(DispatchTask &&task) noexcept = 0;

  //! Invoke the task immediately if the queue uses the current thread. Otherwise, post it.
  //! The immediate execution ignores the suspend or shutdown states.
  virtual void InvokeElsePost(DispatchTask &&task) noexcept = 0;

  //! When called in the context of a task currently run by the queue then it is invoked as a part
  //! of the current task, but after the current task unwinds the call stack.
  //! Otherwise, post it for the asynchronous invocation.
  virtual void DeferElsePost(DispatchTask &&task) noexcept = 0;

  //! True if current task is invoked in context of this dispatch queue.
  virtual bool IsCurrentQueue() noexcept = 0;

  //! True if tasks are invoked in a serial order by the queue.
  virtual bool IsSerial() noexcept = 0;

  //! True if queue is running on current thread or associated with it.
  virtual bool HasThreadAccess() noexcept = 0;

  //! Check if a long running task should yield.
  //! If provided yieldReason is not null, then it is assigned with a reason why the task is asked to yield.
  //! ShouldYield must not be checked at the start of task invocation because trivial implementation of ShouldYield
  //! always returns true and the long running task will never make any progress.
  virtual bool ShouldYield(TaskYieldReason *yieldReason) noexcept = 0;

  //! Start collecting all tasks posted to this queue from the current thread into a new batched task.
  virtual void BeginTaskBatching() noexcept = 0;

  //! Stop batching tasks for the current thread and return the task batch as a single dispatch task.
  virtual DispatchTask EndTaskBatching() noexcept = 0;

  //! Return true if tasks from the current thread are being batched by the dispatch queue.
  virtual bool HasTaskBatching() noexcept = 0;

  //! Tries to lock queue local value (QLV) by swapping it with the provided TLS value.
  //! While the QLV is locked, we can access it through the TLS variable.
  //! Unlocking the QLV swaps it back with the TLS variable.
  //! On the first call the variable will be created by swapLocalValue. When queue is shutdown the variable is destroyed
  //! by swapLocalValue. The TLS variable address corresponding to the QLV is used to define a key for storing QLV.
  virtual bool TryLockQueueLocalValue(SwapDispatchLocalValueCallback swapLocalValue, void **tlsValue) noexcept = 0;

  //! Unlock the queue local value by swapping it back with the TLS value.
  virtual void UnlockLocalValue(void **tlsValue) noexcept = 0;

  //! Suspend task invocation and increment internal suspend count by one.
  //! Note that we do not expose IsSuspeneded() method because the Suspend/Resume
  //! methods can be called from different threads and using IsSuspeneded()
  //! could lead to errors because the status can be changed any time right after the check.
  virtual void Suspend() noexcept = 0;

  //! Decrement the suspend counter and resume task invocation if it is zero.
  //! The Suspend/Resume calls must be balanced. Extra Resume() calls are going to cause a crash.
  virtual void Resume() noexcept = 0;

  //! Shutdown the queue. All new tasks are going to be canceled.
  //! Pending tasks are going to be completed or canceled depending on pendingTaskAction.
  virtual void Shutdown(PendingTaskAction pendingTaskAction) noexcept = 0;

  //! Waits until all pending tasks are completed after shutdown.
  virtual void AwaitTermination() noexcept = 0;

  //! Returns true if the queue has tasks to invoke.
  virtual bool HasTasks() noexcept = 0;

  //! Try to dequeue a task from dispatch queue for processing. It returns true if task is not empty.
  virtual bool TryDequeTask(/*out*/ DispatchTask &task) noexcept = 0;

  //! Invokes the task in the dispatch queue context. It also invokes all deferred tasks created from current task.
  virtual void InvokeTask(
      DispatchTask &&task,
      std::optional<std::chrono::steady_clock::time_point> endTime) noexcept = 0;

  //! Calls ICancellationListener::OnCancel in case if task implements the ICancellationListener interface.
  virtual void CancelTask(DispatchTask &&task) noexcept = 0;
};

//! The interface for dispatch queue static members.
MSO_GUID(IDispatchQueueStatic, "500b28a7-b5ab-4f6b-970e-b556658b6233")
struct IDispatchQueueStatic : IUnknown {
  //! Get the instance of the IDispatchQueueStatic interface.
  LIBLET_PUBLICAPI static IDispatchQueueStatic *Instance() noexcept;

  //! Return dispatch queue which currently running a task on this thread.
  //! If no task running, then it returns a queue with an empty state.
  virtual DispatchQueue CurrentQueue() noexcept = 0;

  //! Get global concurrent queue which is a thin wrapper on top of platform specific thread pool. It is created on
  //! demand.
  virtual DispatchQueue const &ConcurrentQueue() noexcept = 0;

  //! Create new serial DispatchQueue on top of platform specific thread pool.
  virtual DispatchQueue MakeSerialQueue() noexcept = 0;

  //! Create new looper DispatchQueue on top of new std::thread. It owns the thread until shutdown.
  virtual DispatchQueue MakeLooperQueue() noexcept = 0;

  //! Get a dispatch queue for the current UI thread. The result is null if the UI thread has no system UI thread
  //! dispatcher.
  virtual DispatchQueue GetCurrentUIThreadQueue() noexcept = 0;

  //! Create a concurrent queue on top of platform specific thread pool that uses up to maxThreads threads.
  //! If maxThreads is zero, then it creates a concurrent queue that has a predefined limit on concurrently submitted
  //! work items.
  //! If maxThreads is one, then it calls MakeSerialQueue.
  //! If maxThreads is two or more, then it creates a concurrent queue with the maxThreads limit for concurrently
  //! running tasks.
  virtual DispatchQueue MakeConcurrentQueue(uint32_t maxThreads) noexcept = 0;

  //! Create a dispatch queue on top of custom IDispatchQueueScheduler.
  //! The IDispatchQueueScheduler defines how the dispatch queue items are handled.
  virtual DispatchQueue MakeCustomQueue(Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept = 0;
};

//! DispatchTask implementation based on invoke and cancel function objects.
template <typename TInvoke, typename TOnCancel>
struct DispatchTaskImpl final
    : Mso::UnknownObject<Mso::QueryCastHidden<Mso::IVoidFunctor>, Mso::ICancellationListener> {
  template <typename TInvokeArg, typename TOnCancelArg>
  DispatchTaskImpl(TInvokeArg &&invoke, TOnCancelArg &&onCancel) noexcept;
  ~DispatchTaskImpl() noexcept override;

  void Invoke() noexcept override;
  void OnCancel() noexcept override;

 private:
  TInvoke m_invoke;
  TOnCancel m_onCancel;
  std::atomic<bool> m_isCalled{false};
};

//! Dispatch task implementation that runs the same lambda for Invoke() and OnCancel().
template <typename TInvoke>
struct DispatchCleanupTaskImpl final
    : Mso::UnknownObject<Mso::QueryCastHidden<Mso::IVoidFunctor>, Mso::ICancellationListener> {
  template <typename TInvokeArg>
  DispatchCleanupTaskImpl(TInvokeArg &&invoke) noexcept;
  void Invoke() noexcept override;
  void OnCancel() noexcept override;

 private:
  TInvoke m_invoke;
};

//=============================================================================
// Standalone inline implementations
//=============================================================================

template <typename TObject>
inline auto GetRawState(TObject &&obj) noexcept {
  return obj.m_state.GetRaw();
}

template <typename TValue>
void SwapDispatchLocalValue(void **localValue, void **tlsValue) noexcept {
  if (!tlsValue) { // Delete the queue local value when tlsValue is null.
    if (*localValue) {
      delete static_cast<TValue *>(*localValue);
      *localValue = nullptr;
    }
    return;
  } else if (!*tlsValue && !*localValue) { // Initialize queue local value on the first call
    *localValue = new TValue{};
  }
  using std::swap;
  swap(*localValue, *tlsValue);
}

template <typename TInvoke, typename TOnCancel>
inline Mso::VoidFunctor MakeDispatchTask(TInvoke &&invoke, TOnCancel &&onCancel) noexcept {
  using DispatchTaskType = DispatchTaskImpl<std::decay_t<TInvoke>, std::decay_t<TOnCancel>>;
  return Mso::VoidFunctor(
      Mso::Make<DispatchTaskType, IVoidFunctor>(std::forward<TInvoke>(invoke), std::forward<TOnCancel>(onCancel)));
}

template <typename TInvoke>
inline Mso::VoidFunctor MakeDispatchCleanupTask(TInvoke &&invoke) noexcept {
  using DispatchCleanupTaskType = DispatchCleanupTaskImpl<std::decay_t<TInvoke>>;
  return Mso::VoidFunctor(Mso::Make<DispatchCleanupTaskType, IVoidFunctor>(std::forward<TInvoke>(invoke)));
}

//=============================================================================
// DispatchLocalValueGuard inline implementation
//=============================================================================

inline DispatchLocalValueGuard::DispatchLocalValueGuard(IDispatchQueueService *queue, void **tlsValue) noexcept
    : m_queue{queue}, m_tlsValue{tlsValue} {}

inline DispatchLocalValueGuard::~DispatchLocalValueGuard() noexcept {
  if (m_queue) {
    m_queue->UnlockLocalValue(m_tlsValue);
  }
}

inline DispatchLocalValueGuard::operator bool() const noexcept {
  return m_queue != nullptr;
}

//=============================================================================
// DispatchQueue inline implementation
//=============================================================================

inline DispatchQueue::DispatchQueue() noexcept : DispatchQueue{MakeSerialQueue()} {}

inline DispatchQueue::DispatchQueue(std::nullptr_t) noexcept {}

inline DispatchQueue::DispatchQueue(IDispatchQueueService *state, Mso::AttachTagType tag) noexcept
    : m_state{state, tag} {}

inline DispatchQueue::DispatchQueue(Mso::CntPtr<IDispatchQueueService> &&state) noexcept : m_state{std::move(state)} {}

inline /*static*/ DispatchQueue DispatchQueue::CurrentQueue() noexcept {
  return IDispatchQueueStatic::Instance()->CurrentQueue();
}

inline /*static*/ DispatchQueue const &DispatchQueue::ConcurrentQueue() noexcept {
  return IDispatchQueueStatic::Instance()->ConcurrentQueue();
}

inline /*static*/ DispatchQueue DispatchQueue::MakeSerialQueue() noexcept {
  return IDispatchQueueStatic::Instance()->MakeSerialQueue();
}

inline /*static*/ DispatchQueue DispatchQueue::MakeLooperQueue() noexcept {
  return IDispatchQueueStatic::Instance()->MakeLooperQueue();
}

inline /*static*/ DispatchQueue DispatchQueue::GetCurrentUIThreadQueue() noexcept {
  return IDispatchQueueStatic::Instance()->GetCurrentUIThreadQueue();
}

inline /*static*/ DispatchQueue DispatchQueue::MakeConcurrentQueue(uint32_t maxThreads) noexcept {
  return IDispatchQueueStatic::Instance()->MakeConcurrentQueue(maxThreads);
}

inline /*static*/ DispatchQueue DispatchQueue::MakeCustomQueue(
    Mso::CntPtr<IDispatchQueueScheduler> &&scheduler) noexcept {
  return IDispatchQueueStatic::Instance()->MakeCustomQueue(std::move(scheduler));
}

inline DispatchQueue::operator bool() const noexcept {
  return m_state != nullptr;
}

inline void DispatchQueue::Post(DispatchTask &&task) const noexcept {
  m_state->Post(std::move(task));
}

inline void DispatchQueue::InvokeElsePost(DispatchTask &&task) const noexcept {
  m_state->InvokeElsePost(std::move(task));
}

inline void DispatchQueue::DeferElsePost(DispatchTask &&task) const noexcept {
  m_state->DeferElsePost(std::move(task));
}

inline bool DispatchQueue::IsCurrentQueue() const noexcept {
  return m_state->IsCurrentQueue();
}

inline bool DispatchQueue::IsSerial() const noexcept {
  return m_state->IsSerial();
}

inline bool DispatchQueue::HasThreadAccess() const noexcept {
  return m_state->HasThreadAccess();
}

inline bool DispatchQueue::ShouldYield(TaskYieldReason *yieldReason /*= nullptr*/) const noexcept {
  return m_state->ShouldYield(yieldReason);
}

inline DispatchTaskBatch DispatchQueue::StartTaskBatching() const noexcept {
  return DispatchTaskBatch{m_state};
}

template <typename TValue>
inline DispatchLocalValueGuard DispatchQueue::LockLocalValue(TValue **tlsValue) const noexcept {
  if (m_state->TryLockQueueLocalValue(SwapDispatchLocalValue<TValue>, (void **)tlsValue)) {
    return DispatchLocalValueGuard{m_state.Get(), (void **)tlsValue};
  } else {
    return DispatchLocalValueGuard{nullptr, nullptr};
  }
}

inline DispatchSuspendGuard DispatchQueue::Suspend() const noexcept {
  return DispatchSuspendGuard{m_state};
}

inline void DispatchQueue::Shutdown(PendingTaskAction pendingTaskAction) const noexcept {
  m_state->Shutdown(pendingTaskAction);
}

inline void DispatchQueue::AwaitTermination() const noexcept {
  m_state->AwaitTermination();
}

inline bool DispatchQueue::operator==(DispatchQueue const &other) const noexcept {
  return m_state.Get() == other.m_state.Get();
}

inline bool DispatchQueue::operator!=(DispatchQueue const &other) const noexcept {
  return m_state.Get() != other.m_state.Get();
}

//=============================================================================
// DispatchSuspendGuard inline implementation
//=============================================================================

inline DispatchSuspendGuard::DispatchSuspendGuard(std::nullptr_t) noexcept {}

inline DispatchSuspendGuard::DispatchSuspendGuard(Mso::CntPtr<IDispatchQueueService> const &state) noexcept
    : m_state{state} {}

inline DispatchSuspendGuard::DispatchSuspendGuard(DispatchSuspendGuard const &other) noexcept : m_state{other.m_state} {
  if (m_state) {
    m_state->Suspend();
  }
}

inline DispatchSuspendGuard &DispatchSuspendGuard::operator=(DispatchSuspendGuard const &other) noexcept {
  m_state = other.m_state;
  if (m_state) {
    m_state->Suspend();
  }
  return *this;
}

inline DispatchSuspendGuard::~DispatchSuspendGuard() noexcept {
  if (m_state) {
    m_state->Resume();
  }
}

inline DispatchSuspendGuard::operator bool() const noexcept {
  return m_state != nullptr;
}

//=============================================================================
// DispatchTaskBatch inline implementation
//=============================================================================

inline DispatchTaskBatch::DispatchTaskBatch(std::nullptr_t) noexcept {}

inline DispatchTaskBatch::DispatchTaskBatch(Mso::CntPtr<IDispatchQueueService> const &state) noexcept
    : m_state{state} {}

inline DispatchTaskBatch::~DispatchTaskBatch() noexcept {
  if (m_state) {
    m_state->Post(m_state->EndTaskBatching());
  }
}

inline DispatchTaskBatch::operator bool() const noexcept {
  return m_state != nullptr;
}

inline void DispatchTaskBatch::Post() noexcept {
  if (m_state) {
    m_state->Post(m_state->EndTaskBatching());
    m_state = nullptr;
  }
}

inline void DispatchTaskBatch::InvokeElsePost() noexcept {
  if (m_state) {
    m_state->InvokeElsePost(m_state->EndTaskBatching());
    m_state = nullptr;
  }
}

inline void DispatchTaskBatch::DeferElsePost() noexcept {
  if (m_state) {
    m_state->DeferElsePost(m_state->EndTaskBatching());
    m_state = nullptr;
  }
}

inline void DispatchTaskBatch::Cancel() noexcept {
  if (m_state) {
    m_state->CancelTask(m_state->EndTaskBatching());
    m_state = nullptr;
  }
}

//=============================================================================
// DispatchTaskImpl inline implementation
//=============================================================================

template <typename TInvoke, typename TOnCancel>
template <typename TInvokeArg, typename TOnCancelArg>
inline DispatchTaskImpl<TInvoke, TOnCancel>::DispatchTaskImpl(TInvokeArg &&invoke, TOnCancelArg &&onCancel) noexcept
    : m_invoke{std::forward<TInvokeArg>(invoke)}, m_onCancel{std::forward<TOnCancelArg>(onCancel)} {}

template <typename TInvoke, typename TOnCancel>
inline DispatchTaskImpl<TInvoke, TOnCancel>::~DispatchTaskImpl() noexcept {
  if (!m_isCalled) {
    m_onCancel();
  }
}

template <typename T>
inline void MustBeNoExceptVoidFunctor() {
  static_assert(false, __FUNCTION__ ": not a noexcept callable functor returning void");
}

template <typename TInvoke, typename TOnCancel>
inline void DispatchTaskImpl<TInvoke, TOnCancel>::Invoke() noexcept {
  if constexpr (std::is_nothrow_invocable_r_v<void, decltype(m_invoke)>) {
    m_invoke();
    m_isCalled = true;
  } else {
    MustBeNoExceptVoidFunctor<decltype(m_invoke)>();
  }
}

template <typename TInvoke, typename TOnCancel>
inline void DispatchTaskImpl<TInvoke, TOnCancel>::OnCancel() noexcept {
  if constexpr (std::is_nothrow_invocable_r_v<void, decltype(m_onCancel)>) {
    m_onCancel();
    m_isCalled = true;
  } else {
    MustBeNoExceptVoidFunctor<decltype(m_onCancel)>();
  }
}

//=============================================================================
// DispatchCleanupTaskImpl inline implementation
//=============================================================================

template <typename TInvoke>
template <typename TInvokeArg>
inline DispatchCleanupTaskImpl<TInvoke>::DispatchCleanupTaskImpl(TInvokeArg &&invoke) noexcept
    : m_invoke{std::forward<TInvokeArg>(invoke)} {}

template <typename TInvoke>
inline void DispatchCleanupTaskImpl<TInvoke>::Invoke() noexcept {
  if constexpr (std::is_nothrow_invocable_r_v<void, decltype(m_invoke)>) {
    m_invoke();
  } else {
    MustBeNoExceptVoidFunctor<decltype(m_invoke)>();
  }
}

template <typename TInvoke>
inline void DispatchCleanupTaskImpl<TInvoke>::OnCancel() noexcept {
  if constexpr (std::is_nothrow_invocable_r_v<void, decltype(m_invoke)>) {
    m_invoke();
  } else {
    MustBeNoExceptVoidFunctor<decltype(m_invoke)>();
  }
}

} // namespace Mso

#endif // MSO_DISPATCHQUEUE_DISPATCHQUEUE_H
