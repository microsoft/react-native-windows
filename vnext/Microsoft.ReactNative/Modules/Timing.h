// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../../codegen/NativeTimingSpec.g.h"

#include <CppWinRTIncludes.h>
#include <ReactCoreInjection.h>
#include <react/runtime/PlatformTimerRegistry.h>
#include <react/runtime/TimerManager.h>

namespace Microsoft::ReactNative {

typedef winrt::Windows::Foundation::DateTime TDateTime;
typedef winrt::Windows::Foundation::TimeSpan TTimeSpan;

class TimingModule;
struct Timing;

struct Timer {
  Timer(uint32_t id, TDateTime targetTime, TTimeSpan period, bool repeat) {
    Id = id;
    TargetTime = targetTime;
    Period = period;
    Repeat = repeat;
  }

  bool operator<(const Timer &timer) const {
    return timer.TargetTime < TargetTime;
  }

  bool operator==(uint32_t id) const {
    return id == Id;
  }

  uint32_t Id;
  TDateTime TargetTime;
  TTimeSpan Period;
  bool Repeat;
};

class TimerQueue {
 public:
  TimerQueue();

  void Push(uint32_t id, TDateTime targetTime, TTimeSpan period, bool repeat);
  void Pop();
  Timer &Front();
  void Remove(uint32_t id);

  bool IsEmpty();

 private:
  std::vector<Timer> m_timerVector;
};

struct TimerRegistry : public facebook::react::PlatformTimerRegistry {
  static std::unique_ptr<TimerRegistry> CreateTimerRegistry(
      const winrt::Microsoft::ReactNative::IReactPropertyBag &properties) noexcept;

  ~TimerRegistry();

  void createTimer(uint32_t timerID, double delayMS) override;
  void deleteTimer(uint32_t timerID) override;
  void createRecurringTimer(uint32_t timerID, double delayMS) override;

  // When running bridgeless mode timers are managed by TimerManager
  void setTimerManager(std::weak_ptr<facebook::react::TimerManager> timerManager);

  void callTimers(const std::vector<uint32_t> &ids) noexcept;

 private:
  std::weak_ptr<facebook::react::TimerManager> m_timerManager;
  std::shared_ptr<Timing> m_timingModule;
};

REACT_MODULE(Timing)
struct Timing : public std::enable_shared_from_this<Timing> {
  // Spec enforces using double for ids instead of uint32_t
  // using ModuleSpec = ReactNativeSpecs::TimingSpec;

 public:
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void InitializeBridgeless(
      TimerRegistry *timerRegistry,
      const winrt::Microsoft::ReactNative::IReactPropertyBag &properties) noexcept;
  void DetachBridgeless();

  REACT_METHOD(createTimer)
  void createTimer(uint32_t id, double duration, double jsSchedulingTime, bool repeat) noexcept;

  REACT_METHOD(deleteTimer)
  void deleteTimer(uint32_t id) noexcept;
  REACT_METHOD(setSendIdleEvents)
  void setSendIdleEvents(bool sendIdleEvents) noexcept;

 private:
  void createTimerOnQueue(uint32_t id, double duration, double jsSchedulingTime, bool repeat) noexcept;
  void deleteTimerOnQueue(uint32_t id) noexcept;
  void OnTick();
  winrt::Microsoft::ReactNative::ITimer EnsureDispatcherTimer();
  void StartRendering();
  void PostRenderFrame() noexcept;
  void StartDispatcherTimer();
  void StopTicks();

  winrt::Microsoft::ReactNative::ReactContext m_context; // !bridgeless
  TimerRegistry *m_timerRegistry{nullptr}; // bridgeless
  winrt::Microsoft::ReactNative::IReactPropertyBag m_properties{nullptr};
  TimerQueue m_timerQueue;
  xaml::Media::CompositionTarget::Rendering_revoker m_rendering;
  winrt::Microsoft::ReactNative::ITimer m_dispatcherQueueTimer{nullptr};
  winrt::weak_ref<winrt::Microsoft::ReactNative::IReactDispatcher> m_uiDispatcher;
  bool m_usingRendering{false};
  bool m_usePostForRendering{false};
};

} // namespace Microsoft::ReactNative
