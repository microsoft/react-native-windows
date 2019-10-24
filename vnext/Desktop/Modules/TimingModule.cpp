// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#undef Check
using namespace std;

#include <folly/dynamic.h>
#include <cassert>
#include "TimingModule.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

#include "NativeModuleFactories.h"

using namespace facebook::xplat;
using namespace folly;

namespace facebook {
namespace react {

bool operator<(const Timer &leftTimer, const Timer &rightTimer) {
  return rightTimer.DueTime < leftTimer.DueTime;
}

bool operator==(const Timer &leftTimer, const uint64_t id) {
  return id == leftTimer.Id;
}

void TimerQueue::Push(Timer timer) {
  m_timerVector.push_back(timer);
  std::push_heap(m_timerVector.begin(), m_timerVector.end());
}

void TimerQueue::Pop() {
  std::pop_heap(m_timerVector.begin(), m_timerVector.end());
  m_timerVector.pop_back();
}

Timer &TimerQueue::Front() {
  return m_timerVector.front();
}

const Timer &TimerQueue::Front() const {
  return m_timerVector.front();
}

bool TimerQueue::Remove(uint64_t id) {
  // TODO: This is very inefficient, but doing this with a heap is inherently
  // hard. If performance is not good
  //	enough for the scenarios then a different structure is probably needed.
  auto found = std::find(m_timerVector.begin(), m_timerVector.end(), id);
  if (found != m_timerVector.end()) {
    m_timerVector.erase(found);
  } else {
    return false;
  }
  std::make_heap(m_timerVector.begin(), m_timerVector.end());
  return true;
}

bool TimerQueue::IsEmpty() const {
  return m_timerVector.empty();
}

/*static*/ void Timing::ThreadpoolTimerCallback(PTP_CALLBACK_INSTANCE, PVOID Parameter, PTP_TIMER) noexcept {
  static_cast<Timing *>(Parameter)->OnTimerRaised();
}

void Timing::OnTimerRaised() noexcept {
  if (auto inst = m_wkInstance.lock()) {
    if (auto nativeThread = m_nativeThread.lock()) {
      // Make sure we execute it on native thread for native modules
      // Capture weak_ptr "this" because callback will be executed on native
      // thread even if "this" is destroyed.
      nativeThread->runOnQueue([weakThis = std::weak_ptr<Timing>(shared_from_this())]() {
        auto strongThis = weakThis.lock();
        if (!strongThis) {
          return;
        }

        if ((!strongThis->m_threadpoolTimer) || strongThis->m_timerQueue.IsEmpty()) {
          return;
        }

        folly::dynamic readyTimers = folly::dynamic::array();
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

        // Fire timers which will be expired in 10ms
        while (!strongThis->m_timerQueue.IsEmpty() && now_ms > strongThis->m_timerQueue.Front().DueTime - 10ms) {
          // Pop first timer from the queue and add it to list of timers ready
          // to fire
          auto next = strongThis->m_timerQueue.Front();
          strongThis->m_timerQueue.Pop();

          // VSO:1916882 potential overflow
          readyTimers.push_back(next.Id);

          // If timer is repeating push it back onto the queue for the next
          // repetition 'next.Period' being greater than 10ms is intended to
          // prevent infinite loops
          if (next.Repeat)
            strongThis->m_timerQueue.Push(Timer{next.Id, now_ms + next.Period, next.Period, true});
        }

        if (!readyTimers.empty()) {
          if (auto instance = strongThis->m_wkInstance.lock()) {
            instance->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(std::move(readyTimers)));
          } else {
            assert(false && "m_wkInstance.lock failed");
          }
        }

        if (!strongThis->m_timerQueue.IsEmpty()) {
          strongThis->SetKernelTimer(strongThis->m_timerQueue.Front().DueTime);
        } else {
          strongThis->m_dueTime = DateTime::max();
        }
      });
    } else {
      assert(false && "m_nativeThread.lock failed");
    }
  }
}

void Timing::createTimer(
    std::weak_ptr<facebook::react::Instance> instance,
    uint64_t id,
    double duration,
    double jsSchedulingTime,
    bool repeat) noexcept {
  SetInstance(instance);
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

  // Convert double duration to std::chrono::duration
  auto period = TimeSpan{(int64_t)duration};
  // Convert int64_t scheduletime to std::chrono::time_point
  DateTime scheduledTime = DateTime(TimeSpan((int64_t)jsSchedulingTime));
  // Calculate the initial due time -- scheduleTime plus duration
  auto initialDueTime = scheduledTime + period;

  if (scheduledTime + period <= now_ms && !repeat) {
    if (auto inst = m_wkInstance.lock()) {
      inst->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(folly::dynamic::array(id)));
    } else {
      assert(false && "m_wkInstance.lock failed");
    }
    return;
  }

  // Make sure duration is always larger than 16ms to avoid unnecessary wakeups.
  period = TimeSpan{duration < 16 ? 16 : (int64_t)duration};
  m_timerQueue.Push(Timer{id, initialDueTime, period, repeat});

  TimersChanged();
}

void Timing::TimersChanged() noexcept {
  if (m_timerQueue.IsEmpty()) {
    // TimerQueue is empty.
    // Stop the kernel timer only when it is about to fire
    if (KernelTimerIsAboutToFire()) {
      StopKernelTimer();
    }
    return;
  }
  // If front timer has the same target time as ThreadpoolTimer,
  // we will keep ThreadpoolTimer unchanged.
  if (m_timerQueue.Front().DueTime == m_dueTime) {
    // do nothing
  }
  // If current front timer's due time is earlier than current
  // ThreadpoolTimer's, we need to reset the ThreadpoolTimer to current front
  // timer
  else if (m_timerQueue.Front().DueTime < m_dueTime) {
    SetKernelTimer(m_timerQueue.Front().DueTime);
  }
  // If current front timer's due time is later than current kernel timer's,
  // we will reset kernel timer only when it is about to fire
  else if (KernelTimerIsAboutToFire()) {
    SetKernelTimer(m_timerQueue.Front().DueTime);
  }
}

bool Timing::KernelTimerIsAboutToFire() noexcept {
  // Here we assume if kernel timer is going to fire within 2 frames (about
  // 33ms), we return true I am not sure the 2 frames assumption is good enough.
  // We may need adjustment after performance analysis.
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  if (m_dueTime - now_ms <= 33ms)
    return true;
  return false;
}

void Timing::SetInstance(std::weak_ptr<facebook::react::Instance> instance) noexcept {
  if (m_wkInstance.expired())
    m_wkInstance = instance;
}

void Timing::SetKernelTimer(DateTime dueTime) noexcept {
  m_dueTime = dueTime;
  FILETIME FileDueTime;
  ULARGE_INTEGER ulDueTime;
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  TimeSpan period = dueTime - now_ms;
  ulDueTime.QuadPart = (ULONGLONG) - (period.count() * 10000);
  FileDueTime.dwHighDateTime = ulDueTime.HighPart;
  FileDueTime.dwLowDateTime = ulDueTime.LowPart;

  if (!m_threadpoolTimer) {
    InitializeKernelTimer();
  }

  SetThreadpoolTimer(m_threadpoolTimer, &FileDueTime, 0, 0);
}

void Timing::InitializeKernelTimer() noexcept {
  // Create ThreadPoolTimer
  m_threadpoolTimer = CreateThreadpoolTimer(&Timing::ThreadpoolTimerCallback, static_cast<PVOID>(this), NULL);
  assert(m_threadpoolTimer && "CreateThreadpoolTimer failed.");
}

void Timing::deleteTimer(uint64_t id) noexcept {
  if (m_timerQueue.IsEmpty())
    return;
  if (m_timerQueue.Remove(id)) {
    TimersChanged();
  }
}

void Timing::StopKernelTimer() noexcept {
  // Cancel pending callbacks
  SetThreadpoolTimer(m_threadpoolTimer, NULL, 0, 0);
  m_dueTime = DateTime::max();
}

void Timing::setSendIdleEvents(bool /*sendIdleEvents*/) noexcept {
  // It seems we don't need this API. Leave it empty for now.
  assert(false && "not implemented");
}

Timing::~Timing() {
  if (m_threadpoolTimer) {
    StopKernelTimer();
    WaitForThreadpoolTimerCallbacks(m_threadpoolTimer, true);
    CloseThreadpoolTimer(m_threadpoolTimer);
  }
}

TimingModule::TimingModule(std::shared_ptr<Timing> &&timing) : m_timing(std::move(timing)) {}

std::string TimingModule::getName() {
  return "Timing";
}

std::map<std::string, dynamic> TimingModule::getConstants() noexcept {
  return {};
}

std::vector<module::CxxModule::Method> TimingModule::getMethods() noexcept {
  return {
      Method(
          "createTimer",
          [this](dynamic args) // int64_t id, int64_t duration, double
                               // jsSchedulingTime, bool repeat
          {
            m_timing->createTimer(
                getInstance(),
                jsArgAsInt(args, 0),
                jsArgAsDouble(args, 1),
                jsArgAsDouble(args, 2),
                jsArgAsBool(args, 3));
          }),
      Method(
          "deleteTimer",
          [this](dynamic args) // int64_t code, const std::string& reason,
                               // int64_t id
          { m_timing->deleteTimer(jsArgAsInt(args, 0)); }),
      Method(
          "setSendIdleEvents",
          [this](dynamic args) // const std::string& message, int64_t id
          { m_timing->setSendIdleEvents(jsArgAsBool(args, 0)); }),
  };
}

std::unique_ptr<facebook::xplat::module::CxxModule> CreateTimingModule(
    const std::shared_ptr<facebook::react::MessageQueueThread> &nativeThread) noexcept {
  auto module = std::make_unique<TimingModule>(std::make_shared<Timing>(nativeThread));
  return std::move(module);
}

} // namespace react
} // namespace facebook
