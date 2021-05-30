// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_EVENTWAITHANDLE_EVENTWAITHANDLE_H
#define MSO_EVENTWAITHANDLE_EVENTWAITHANDLE_H

#include <chrono>
#include "compilerAdapters/functionDecorations.h"
#include "smartPtr/cntPtr.h"

namespace Mso {

//! Shared event wait handle interface used by ManualResetEvent and AutoResetEvent.
//! Since, ManualResetEvent and AutoResetEvent are most commonly used between different threads, we used
//! shared ownership based on ref counting to ensure proper lifetime of the synchronization events.
struct IEventWaitHandle : Mso::IRefCounted {
  virtual void Set() const noexcept = 0;
  virtual void Reset() const noexcept = 0;
  virtual bool Wait() const noexcept = 0;
  virtual bool WaitFor(const std::chrono::milliseconds &waitDuration) const noexcept = 0;
};

//! State of the IEventWaitHandle.
enum class EventWaitHandleState : int32_t {
  //! Threads are blocked when calling Wait() or WaitFor() methods.
  NotSet,

  //! Threads are allowed to proceed or getting unblocked when calling Wait() or WaitFor() methods.
  IsSet,
};

//! Notifies one or more waiting threads that an event has occurred. This class cannot be inherited.
//!
//! Signal the ManualResetEvent by calling its Set method. A ManualResetEvent object remains signaled until
//! its Reset method is called. Any number of waiting threads, or threads that wait on the event after it has
//! been signaled, can be released while the object's state is signaled. Until it is reset, by calling its
//! Reset method, threads that wait on the event handle proceed immediately without blocking.
//!
//! A manual reset event acts like the gate of a corral. When the event is not signaled, threads that wait on
//! it block, like horses in a corral. When the event is signaled, by calling its Set method, all waiting
//! threads are free to proceed. The event remains signaled until its Reset method is called. This makes the
//! manual reset event an ideal way to hold up threads that need to wait until one thread finishes a task.
//!
//! Like horses leaving a corral, it takes time for the released threads to be scheduled by the operating
//! system and to resume execution. If the Reset method is called before all the threads have resumed
//! execution, the remaining threads once again block. Which threads resume and which threads block depends
//! on random factors like the load on the system, the number of threads waiting for the scheduler, and
//! so on. This is not a problem if the thread that signals the event ends task after signaling, which is the
//! most common usage pattern. If you want the thread that signaled the event to begin a new task after all
//! the waiting threads have resumed, you must block it until all the waiting threads have resumed.
//! Otherwise, you have a race condition, and the behavior of your code is unpredictable.
class ManualResetEvent final {
 public:
  //! Creates new ManualResetEvent with the non-signaling state.
  ManualResetEvent() noexcept : ManualResetEvent{EventWaitHandleState::NotSet} {}

  //! Creates new ManualResetEvent with the requested signaling state.
  LIBLET_PUBLICAPI explicit ManualResetEvent(EventWaitHandleState state) noexcept;

  //! Creates new ManualResetEvent with the provided handle.
  ManualResetEvent(IEventWaitHandle &handle) noexcept : m_handle{&handle} {}

  ManualResetEvent(const ManualResetEvent &) = default;
  ManualResetEvent &operator=(const ManualResetEvent &) = default;

  //! No move semantic
  ManualResetEvent(ManualResetEvent &&) = delete;
  ManualResetEvent &operator=(ManualResetEvent &&) = delete;

  //! Sets the state of the event to signaled, which allows one or more waiting threads to proceed.
  void Set() const noexcept {
    m_handle->Set();
  }

  //! Sets the state of the event to non-signaled state, which causes waiting threads to block.
  void Reset() const noexcept {
    m_handle->Reset();
  }

  //! Blocks thread indefinitely and waits for the event signaling state.
  bool Wait() const noexcept {
    return m_handle->Wait();
  }

  //! Blocks thread for waitDuration time and waits for the event signaling state.
  template <typename TRep, typename TPeriod>
  bool WaitFor(const std::chrono::duration<TRep, TPeriod> &waitDuration) const noexcept {
    return m_handle->WaitFor(std::chrono::duration_cast<std::chrono::milliseconds>(waitDuration));
  }

  //! Gets the internal event handle.
  IEventWaitHandle &GetHandle() const noexcept {
    return *m_handle;
  }

 private:
  Mso::CntPtr<IEventWaitHandle> m_handle;
};

//! Notifies a waiting thread that an event has occurred. This class cannot be inherited.
//!
//! Signal the event by calling its Set method. An AutoResetEvent object is automatically reset to
//! non-signaled by the system after a single waiting thread has been released. If no threads are waiting,
//! the event object's state remains signaled.
//!
//! Automatic reset events are usually used to provide exclusive access to a resource for a single thread at
//! a time. A thread requests the resource by calling the Wait method. If no other thread is holding the wait
//! handle, the method returns true and the calling thread has control of the resource.
//!
//! (!)Important
//! As with all synchronization mechanisms, you must ensure that all code paths wait on the appropriate wait
//! handle before accessing a protected resource. Thread synchronization is cooperative.
//!
//! If an automatic reset event is signaled when no threads are waiting, it remains signaled until a thread
//! attempts to wait on it. The event releases the thread and immediately resets, blocking subsequent threads.
class AutoResetEvent final {
 public:
  //! Creates new AutoResetEvent with the non-signaling state.
  AutoResetEvent() noexcept : AutoResetEvent{EventWaitHandleState::NotSet} {}

  //! Creates new AutoResetEvent with the requested signaling state.
  LIBLET_PUBLICAPI explicit AutoResetEvent(EventWaitHandleState state) noexcept;

  //! Creates new AutoResetEvent with the provided handle. Crash if handle is null.
  AutoResetEvent(IEventWaitHandle &handle) noexcept : m_handle{&handle} {}

  AutoResetEvent(const AutoResetEvent &) = default;
  AutoResetEvent &operator=(const AutoResetEvent &) = default;

  // No move semantic
  AutoResetEvent(AutoResetEvent &&) = delete;
  AutoResetEvent &operator=(AutoResetEvent &&) = delete;

  //! Sets the state of the event to signaled, which allows at most one waiting thread to proceed.
  void Set() const noexcept {
    m_handle->Set();
  }

  //! Sets the state of the event to non-signaled state, which causes waiting threads to block.
  void Reset() const noexcept {
    m_handle->Reset();
  }

  //! Blocks thread indefinitely and waits for the event signaling state.
  void Wait() const noexcept {
    m_handle->Wait();
  }

  //! Blocks thread for waitDuration time and waits for the event signaling state.
  template <typename TRep, typename TPeriod>
  bool WaitFor(const std::chrono::duration<TRep, TPeriod> &waitDuration) const noexcept {
    return m_handle->WaitFor(std::chrono::duration_cast<std::chrono::milliseconds>(waitDuration));
  }

  //! Gets the internal event handle.
  IEventWaitHandle &GetHandle() const noexcept {
    return *m_handle;
  }

 private:
  Mso::CntPtr<IEventWaitHandle> m_handle;
};

} // namespace Mso

#endif // MSO_EVENTWAITHANDLE_EVENTWAITHANDLE_H
