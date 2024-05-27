// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../../codegen/NativeTimingSpec.g.h"

#include <ReactCoreInjection.h>

namespace Microsoft::ReactNative {

typedef winrt::Windows::Foundation::DateTime TDateTime;
typedef winrt::Windows::Foundation::TimeSpan TTimeSpan;

class TimingModule;

struct Timer {
  Timer(int64_t id, TDateTime targetTime, TTimeSpan period, bool repeat) {
    Id = id;
    TargetTime = targetTime;
    Period = period;
    Repeat = repeat;
  }

  bool operator<(const Timer &timer) const {
    return timer.TargetTime < TargetTime;
  }

  bool operator==(int64_t id) const {
    return id == Id;
  }

  int64_t Id;
  TDateTime TargetTime;
  TTimeSpan Period;
  bool Repeat;
};

class TimerQueue {
 public:
  TimerQueue();

  void Push(int64_t id, TDateTime targetTime, TTimeSpan period, bool repeat);
  void Pop();
  Timer &Front();
  void Remove(int64_t id);

  bool IsEmpty();

 private:
  std::vector<Timer> m_timerVector;
};

REACT_MODULE(Timing)
struct Timing : public std::enable_shared_from_this<Timing> {
  using ModuleSpec = ReactNativeSpecs::TimingSpec;

 public:
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(createTimer)
  void createTimer(double id, double duration, double jsSchedulingTime, bool repeat) noexcept;

  REACT_METHOD(deleteTimer)
  void deleteTimer(double id) noexcept;
  REACT_METHOD(setSendIdleEvents)
  void setSendIdleEvents(bool sendIdleEvents) noexcept;

 private:
  void createTimerOnQueue(int64_t id, double duration, double jsSchedulingTime, bool repeat) noexcept;
  void deleteTimerOnQueue(int64_t id) noexcept;
  void OnTick();
  winrt::dispatching::DispatcherQueueTimer EnsureDispatcherTimer();
  void StartRendering();
  void PostRenderFrame() noexcept;
  void StartDispatcherTimer();
  void StopTicks();

  React::ReactContext m_context;
  TimerQueue m_timerQueue;
  xaml::Media::CompositionTarget::Rendering_revoker m_rendering;
  winrt::dispatching::DispatcherQueueTimer m_dispatcherQueueTimer{nullptr};
  bool m_usingRendering{false};
  bool m_usePostForRendering{false};
};

} // namespace Microsoft::ReactNative
