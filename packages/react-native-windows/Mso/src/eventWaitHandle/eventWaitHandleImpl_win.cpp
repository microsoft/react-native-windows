// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <synchapi.h>
#include <algorithm>
#include "eventWaitHandleImpl.h"

//! The EventWaitHandle uses SRWLOCK and CONDITION_VARIABLE.
//! These APIs were added in Windows Vista and considered to be more
//! lightweight when events created with CreateEventEx Windows API.

namespace Mso {

namespace {

// Mutex class based on SRWLOCK that meets BasicLockable standard requirements.
struct SRWMutex {
  _Requires_lock_not_held_(this->Handle) _Acquires_lock_(this->Handle) void lock() noexcept {
    AcquireSRWLockExclusive(&Handle);
  }

  _Requires_lock_held_(this->Handle) _Releases_lock_(this->Handle) void unlock() noexcept {
    ReleaseSRWLockExclusive(&Handle);
  }

  SRWLOCK Handle{SRWLOCK_INIT};
};

// Windows CONDITION_VARIABLE wrapper
struct ConditionVariable {
  void NotifyOne() noexcept {
    WakeConditionVariable(&m_cond);
  }

  void NotifyAll() noexcept {
    WakeAllConditionVariable(&m_cond);
  }

  bool WaitUntil(SRWMutex &mutex, WaitTimePoint &waitTimePoint) noexcept {
    if (!SleepConditionVariableSRW(&m_cond, &mutex.Handle, GetWaitTimeInMs(waitTimePoint), 0)) {
      VerifyElseCrashSzTag(
          GetLastError() == ERROR_TIMEOUT, "SleepConditionVariableSRW failed.", 0x026e3490 /* tag_c19sq */);
      return false;
    }

    return true;
  }

 private:
  static uint32_t GetWaitTimeInMs(WaitTimePoint &waitTimePoint) noexcept {
    if (waitTimePoint.IsInfinite) {
      return INFINITE;
    }

    // For the very first time here we use duration provided by user.
    // If we get here again, then it means that the condition_variable was woken up
    // due to sporatic wakeups, and we have to recalculate the wait duration.
    if (waitTimePoint.ShouldUpdateWaitDuration) {
      using namespace std::chrono;
      auto timeLeft = duration_cast<milliseconds>(waitTimePoint.WaitUntil - system_clock::now());
      // Make sure that we do not have negative duration because we waited beyond the target time point,
      // or because system_clock::now() gave inacccurate time due to internal implementation.
      waitTimePoint.WaitDuration = std::max(timeLeft, milliseconds{0});
    }

    waitTimePoint.ShouldUpdateWaitDuration = true;

    return static_cast<uint32_t>(waitTimePoint.WaitDuration.count());
  }

 private:
  CONDITION_VARIABLE m_cond{CONDITION_VARIABLE_INIT};
};

} // namespace

LIBLET_PUBLICAPI ManualResetEvent::ManualResetEvent(EventWaitHandleState state) noexcept
    : m_handle{Mso::Make<EventWaitHandle<SRWMutex, ConditionVariable>>(/*isAutoReset:*/ false, state)} {}

LIBLET_PUBLICAPI AutoResetEvent::AutoResetEvent(EventWaitHandleState state) noexcept
    : m_handle{Mso::Make<EventWaitHandle<SRWMutex, ConditionVariable>>(/*isAutoReset:*/ true, state)} {}

} // namespace Mso
