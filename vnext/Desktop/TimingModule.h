// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <InstanceManager.h>

#include <memory>
#include <vector>
#include <chrono>

#include <windows.h>

namespace facebook {
namespace react {


using DateTime = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
using TimeSpan = std::chrono::milliseconds;

// Timer struct which holds timer id, target time, period between target time and scheduling time and whether it needs to be repeated.
// Example:
//           std::chrono::time_point targetTime = std::chrono::system_clock::now()+100ms;
//           Timer timer = {12345, targetTime, 100ms, false};
struct Timer
{
  uint64_t Id;
  DateTime DueTime;
  TimeSpan Period;
  bool Repeat;
};

// operator for push_heap and pop_heap
bool operator<(const Timer& leftTimer, const Timer& rightTimer);

// operator for find
bool operator==(const Timer& leftTimer, const uint64_t id);

// This class is a implementation of max heap to store Timer objects.
// The front timer has the smallest due time.
// Example:
//           TimerQueue tq;
//           tq.Push(Timer{1234, now()+100ms, 100ms, false});
//           tq.Push(Timer{1235, now()+20ms, 20ms, false});
//           tq.Push(Timer{1236, now()+50ms, 50ms, false});
//           tq.Pop(); //pops timer id: 1235
//           printf("%u", tq.Front().Id); // print 1236
class TimerQueue
{
public:
  void Push(Timer timer);
  void Pop();
  Timer& Front();
  const Timer& Front() const;
  bool Remove(uint64_t id);
  bool IsEmpty() const;
private:
  // This vector is maintained as a max heap, where the front Timer has the smallest due time.
  std::vector<Timer> m_timerVector;
};

// Helper class which implements createTimer, deleteTimer and setSendIdleEvents for actual TimingModule
// Example:
//           Timing timing;
//           timing.createTimer(instance, id, duration, jsScheduleTime, repeat);
//           timing.delete(id);
class Timing : public std::enable_shared_from_this<Timing>
{
public:
  Timing(const std::shared_ptr<facebook::react::MessageQueueThread>& nativeThread) : m_nativeThread(nativeThread){}
  ~Timing();
  void createTimer(std::weak_ptr<facebook::react::Instance> instance, uint64_t id, double duration, double jsSchedulingTime, bool repeat) noexcept;
  void deleteTimer(uint64_t id) noexcept;
  void setSendIdleEvents(bool sendIdleEvents) noexcept;

private:
  static VOID CALLBACK ThreadpoolTimerCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_TIMER Timer) noexcept;
  void OnTimerRaised() noexcept;
  void SetInstance(std::weak_ptr<facebook::react::Instance> instance) noexcept;
  void SetKernelTimer(DateTime dueTime) noexcept;
  void InitializeKernelTimer() noexcept;
  void TimersChanged() noexcept;
  void StopKernelTimer() noexcept;
  bool KernelTimerIsAboutToFire() noexcept;
  TimerQueue m_timerQueue;
  PTP_TIMER m_threadpoolTimer = NULL;
  DateTime m_dueTime;

  std::weak_ptr<facebook::react::Instance> m_wkInstance;
  std::weak_ptr<facebook::react::MessageQueueThread> m_nativeThread;

};

// Native timing module class. It communicates with JS side to manage timers.
// Example:
//           // On JS side
//           const {Timing} = require('NativeModules');
//           Timing.createTimer(id, duration || 0, Date.now(), /* recurring */ false);
//           Timing.deleteTimer(timerID);
class TimingModule : public facebook::xplat::module::CxxModule
{
public:
  TimingModule(std::shared_ptr<Timing>&& timing);
  std::string getName() override;
  virtual std::map<std::string, folly::dynamic> getConstants() noexcept override;
  virtual std::vector<Method> getMethods() noexcept override;

private:
  std::shared_ptr<Timing> m_timing;
};

} }
