// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../../codegen/NativeTimingSpec.g.h"

#include <InstanceManager.h>
#include <NativeModules.h>
#include <cxxreact/MessageQueueThread.h>

#include <chrono>
#include <memory>
#include <vector>

#include <windows.h>

namespace facebook::react {

using DateTime = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
using TimeSpan = std::chrono::milliseconds;

// Timer struct which holds timer id, target time, period between target time
// and scheduling time and whether it needs to be repeated. Example:
//           std::chrono::time_point targetTime =
//           std::chrono::system_clock::now()+100ms; Timer timer = {12345,
//           targetTime, 100ms, false};
struct Timer {
  uint64_t Id;
  DateTime DueTime;
  TimeSpan Period;
  bool Repeat;
};

// operator for push_heap and pop_heap
bool operator<(const Timer &leftTimer, const Timer &rightTimer);

// operator for find
bool operator==(const Timer &leftTimer, const uint64_t id);

// This class is a implementation of max heap to store Timer objects.
// The front timer has the smallest due time.
// Example:
//           TimerQueue tq;
//           tq.Push(Timer{1234, now()+100ms, 100ms, false});
//           tq.Push(Timer{1235, now()+20ms, 20ms, false});
//           tq.Push(Timer{1236, now()+50ms, 50ms, false});
//           tq.Pop(); //pops timer id: 1235
//           printf("%u", tq.Front().Id); // print 1236
class TimerQueue {
 public:
  void Push(Timer timer);
  void Pop();
  Timer &Front();
  const Timer &Front() const;
  bool Remove(uint64_t id);
  bool IsEmpty() const;

 private:
  // This vector is maintained as a max heap, where the front Timer has the
  // smallest due time.
  std::vector<Timer> m_timerVector;
};

REACT_MODULE(Timing)
struct Timing : public std::enable_shared_from_this<Timing> {
  ~Timing();

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(createTimer)
  void createTimer(uint64_t id, double duration, double jsSchedulingTime, bool repeat) noexcept;

  REACT_METHOD(deleteTimer)
  void deleteTimer(uint64_t id) noexcept;

  REACT_METHOD(setSendIdleEvents)
  void setSendIdleEvents(bool sendIdleEvents) noexcept;

 private:
  static void CALLBACK
  ThreadpoolTimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_TIMER Timer) noexcept;
  void OnTimerRaised() noexcept;
  void SetKernelTimer(DateTime dueTime) noexcept;
  void InitializeKernelTimer() noexcept;
  void TimersChanged() noexcept;
  void StopKernelTimer() noexcept;
  bool KernelTimerIsAboutToFire() noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  TimerQueue m_timerQueue;
  PTP_TIMER m_threadpoolTimer{};
  DateTime m_dueTime;
};

} // namespace facebook::react
