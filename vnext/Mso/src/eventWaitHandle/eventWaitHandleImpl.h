// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include <limits>
#include <mutex>
#include <optional>
#include "eventWaitHandle/eventWaitHandle.h"
#include "object/refCountedObject.h"

namespace Mso {

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

struct WaitTimePoint {
  bool IsInfinite{false};
  bool ShouldUpdateWaitDuration{false};
  std::chrono::milliseconds WaitDuration{};
  TimePoint WaitUntil{};
};

// Implementation of the IEventWaitHandle interface
template <class TMutex, class TConditionVariable>
class EventWaitHandle final : public Mso::RefCountedObject<IEventWaitHandle> {
 public:
  EventWaitHandle(bool isAutoReset, EventWaitHandleState state) noexcept : m_isAutoReset{isAutoReset}, m_state{state} {}

 public: // IEventWaitHandle
  void Set() const noexcept override {
    std::lock_guard<TMutex> lock{m_mutex};
    m_state = EventWaitHandleState::IsSet;

    // Notify under the lock to avoid missed signals in case if Wait loop is between
    // checking variable and calling wait for signal API.
    if (m_isAutoReset) {
      m_cond.NotifyOne();
    } else {
      m_cond.NotifyAll();
    }
  }

  void Reset() const noexcept override {
    std::lock_guard<TMutex> lock{m_mutex};
    m_state = EventWaitHandleState::NotSet;
  }

  bool Wait() const noexcept override {
    WaitTimePoint waitTimePoint{};
    waitTimePoint.IsInfinite = true;
    return WaitUntil(waitTimePoint);
  }

  bool WaitFor(const std::chrono::milliseconds &waitDuration) const noexcept override {
    VerifyElseCrashSzTag(
        waitDuration.count() < std::numeric_limits<uint32_t>::max(),
        "waitDuration must not exceed uint32_t size for milliseconds.",
        0x026e348c /* tag_c19sm */);

    auto now = std::chrono::system_clock::now();

    WaitTimePoint waitTimePoint{};
    waitTimePoint.WaitDuration = waitDuration;
    waitTimePoint.WaitUntil = now + waitDuration;
    VerifyElseCrashSzTag(
        waitTimePoint.WaitUntil >= now, "waitDuration causes clock overflow", 0x026e348d /* tag_c19sn */);

    return WaitUntil(waitTimePoint);
  }

 private:
  bool WaitUntil(WaitTimePoint &timePoint) const noexcept {
    std::lock_guard<TMutex> lock{m_mutex};
    while (m_state != EventWaitHandleState::IsSet) {
      if (!m_cond.WaitUntil(m_mutex, timePoint)) {
        return false;
      }
    }

    if (m_isAutoReset) {
      m_state = EventWaitHandleState::NotSet;
    }

    return true;
  }

 private:
  mutable TMutex m_mutex;
  mutable TConditionVariable m_cond;
  const bool m_isAutoReset;
  mutable EventWaitHandleState m_state;
};

} // namespace Mso
