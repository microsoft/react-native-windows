// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#undef Check

#include "Timing.h"

#include <InstanceManager.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
#include <XamlUtils.h>

#include <unknwnbase.h>

using namespace facebook::xplat;
using namespace folly;
namespace winrt {
using namespace Windows::Foundation;
using namespace xaml::Media;
} // namespace winrt
using namespace std;

namespace Microsoft::ReactNative {

//
// IsAnimationFrameRequest
//
static bool IsAnimationFrameRequest(TTimeSpan period, bool repeat) {
  return !repeat && period == std::chrono::milliseconds(1);
}

winrt::Windows::Foundation::TimeSpan TimeSpanFromMs(double ms) {
  std::chrono::milliseconds dur((int64_t)ms);
  return dur;
}

//
// TimerQueue
//

TimerQueue::TimerQueue() {
  std::make_heap(m_timerVector.begin(), m_timerVector.end());
}

void TimerQueue::Push(uint32_t id, TDateTime targetTime, TTimeSpan period, bool repeat) {
  m_timerVector.emplace_back(id, targetTime, period, repeat);
  std::push_heap(m_timerVector.begin(), m_timerVector.end());
}

void TimerQueue::Pop() {
  std::pop_heap(m_timerVector.begin(), m_timerVector.end());
  m_timerVector.pop_back();
}

Timer &TimerQueue::Front() {
  return m_timerVector.front();
}

void TimerQueue::Remove(uint32_t id) {
  // TODO: This is very inefficient, but doing this with a heap is inherently
  // hard. If performance is not good
  //	enough for the scenarios then a different structure is probably needed.
  auto found = std::find(m_timerVector.begin(), m_timerVector.end(), id);
  if (found != m_timerVector.end())
    m_timerVector.erase(found);

  std::make_heap(m_timerVector.begin(), m_timerVector.end());
}

bool TimerQueue::IsEmpty() {
  return m_timerVector.empty();
}

std::unique_ptr<TimerRegistry> TimerRegistry::CreateTimerRegistry(
    const winrt::Microsoft::ReactNative::IReactPropertyBag &properties) noexcept {
  auto registry = std::make_unique<TimerRegistry>();
  registry->m_timingModule = std::make_shared<Timing>();
  registry->m_timingModule->InitializeBridgeless(registry.get(), properties);
  return registry;
}

TimerRegistry::~TimerRegistry() {
  m_timingModule->DetachBridgeless();
}

double SchedulingTimeNow() {
  const int64_t msFrom1601to1970 = 11644473600000;
  return std::chrono::duration<double, std::milli>(TDateTime::clock::now().time_since_epoch()).count() -
      msFrom1601to1970;
}

void TimerRegistry::createTimer(uint32_t timerID, double delayMS) {
  m_timingModule->createTimer(timerID, delayMS, SchedulingTimeNow(), false);
}
void TimerRegistry::deleteTimer(uint32_t timerID) {
  m_timingModule->deleteTimer(timerID);
}
void TimerRegistry::createRecurringTimer(uint32_t timerID, double delayMS) {
  m_timingModule->createTimer(timerID, delayMS, SchedulingTimeNow(), true);
}

void TimerRegistry::callTimers(const vector<uint32_t> &ids) noexcept {
#ifdef USE_FABRIC
  if (auto timerManager = m_timerManager.lock()) {
    for (auto id : ids) {
      timerManager->callTimer(id);
    }
  }
#endif
}

// When running bridgeless mode timers are managed by TimerManager
void TimerRegistry::setTimerManager(std::weak_ptr<facebook::react::TimerManager> timerManager) {
#ifdef USE_FABRIC
  m_timerManager = timerManager;
#endif
}

//
// Timing
//

void Timing::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_properties = reactContext.Properties().Handle();
  m_usePostForRendering = !xaml::TryGetCurrentUwpXamlApplication();
  m_uiDispatcher = m_context.UIDispatcher().Handle();
}

void Timing::InitializeBridgeless(
    TimerRegistry *timerRegistry,
    const winrt::Microsoft::ReactNative::IReactPropertyBag &properties) noexcept {
  m_timerRegistry = timerRegistry;
  m_properties = properties;
  m_usePostForRendering = !xaml::TryGetCurrentUwpXamlApplication();
  m_uiDispatcher = {properties.Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                        .try_as<winrt::Microsoft::ReactNative::IReactDispatcher>()};
}

void Timing::DetachBridgeless() {
  m_timerRegistry = nullptr;
}

void Timing::OnTick() {
  vector<uint32_t> readyTimers;
  auto now = TDateTime::clock::now();

  auto emittedAnimationFrame = false;
  while (!m_timerQueue.IsEmpty() && m_timerQueue.Front().TargetTime < now) {
    // Pop first timer from the queue and add it to list of timers ready to fire
    Timer next = m_timerQueue.Front();
    m_timerQueue.Pop();
    readyTimers.push_back(next.Id);

    // If timer is repeating push it back onto the queue for the next repetition
    if (next.Repeat)
      m_timerQueue.Push(next.Id, now + next.Period, next.Period, true);
    else if (IsAnimationFrameRequest(next.Period, next.Repeat))
      emittedAnimationFrame = true;
  }

  if (m_timerQueue.IsEmpty()) {
    StopTicks();
  } else if (m_usePostForRendering && m_usingRendering && emittedAnimationFrame) {
    PostRenderFrame(); // Repost
  } else if (!m_usingRendering || !emittedAnimationFrame) {
    // If we're using a rendering callback, check if any animation frame
    // requests were emitted in this tick. If not, start the dispatcher timer.
    // This extra frame gives JS a chance to enqueue an additional animation
    // frame request before switching tick schedulers.
    StartDispatcherTimer();
  }

  if (!readyTimers.empty()) {
    if (m_context) {
      winrt::Microsoft::ReactNative::JSValueArray readyTimersJsArray;
      for (auto id : readyTimers) {
        readyTimersJsArray.push_back({id});
      }
      m_context.CallJSFunction(
          L"JSTimers", L"callTimers", winrt::Microsoft::ReactNative::JSValueArray{std::move(readyTimersJsArray)});
    } else if (m_timerRegistry) {
      m_timerRegistry->callTimers(readyTimers);
    }
  }
}

winrt::Microsoft::ReactNative::ITimer Timing::EnsureDispatcherTimer() {
  if (!m_dispatcherQueueTimer) {
    m_dispatcherQueueTimer = winrt::Microsoft::ReactNative::Timer::Create(m_properties);
    m_dispatcherQueueTimer.Tick([wkThis = std::weak_ptr(this->shared_from_this())](auto &&...) {
      if (auto pThis = wkThis.lock()) {
        pThis->OnTick();
      }
    });
  }

  return m_dispatcherQueueTimer;
}

void Timing::PostRenderFrame() noexcept {
  assert(m_usePostForRendering);
  m_usingRendering = true;
  if (auto uiDispatcher = m_uiDispatcher.get()) {
    uiDispatcher.Post([wkThis = std::weak_ptr(this->shared_from_this())]() {
      if (auto pThis = wkThis.lock()) {
        pThis->m_usingRendering = false;
        pThis->OnTick();
      }
    });
  }
}

void Timing::StartRendering() {
  if (m_dispatcherQueueTimer)
    m_dispatcherQueueTimer.Stop();

  if (m_usePostForRendering) {
    PostRenderFrame();
    return;
  }
  m_rendering.revoke();
  m_usingRendering = true;
  m_rendering = xaml::Media::CompositionTarget::Rendering(
      winrt::auto_revoke,
      [wkThis = std::weak_ptr(this->shared_from_this())](
          const winrt::IInspectable &, const winrt::IInspectable & /*args*/) {
        if (auto pThis = wkThis.lock()) {
          pThis->OnTick();
        }
      });
}

void Timing::StartDispatcherTimer() {
  const auto &nextTimer = m_timerQueue.Front();
  m_rendering.revoke();
  m_usingRendering = false;
  auto timer = EnsureDispatcherTimer();
  timer.Interval(std::max(nextTimer.TargetTime - TDateTime::clock::now(), TTimeSpan::zero()));
  timer.Start();
}

void Timing::StopTicks() {
  m_rendering.revoke();
  m_usingRendering = false;
  if (m_dispatcherQueueTimer)
    m_dispatcherQueueTimer.Stop();
}

void Timing::createTimerOnQueue(uint32_t id, double duration, double jsSchedulingTime, bool repeat) noexcept {
  if (duration == 0 && !repeat) {
    if (m_context) {
      m_context.CallJSFunction(
          L"JSTimers",
          L"callTimers",
          winrt::Microsoft::ReactNative::JSValueArray{winrt::Microsoft::ReactNative::JSValueArray{id}});
    } else if (m_timerRegistry) {
      m_timerRegistry->callTimers({id});
    }

    return;
  }

  // Convert double duration in ms to TimeSpan
  auto period = TimeSpanFromMs(duration);
  const int64_t msFrom1601to1970 = 11644473600000;
  TDateTime scheduledTime(TimeSpanFromMs(jsSchedulingTime + msFrom1601to1970));
  auto initialTargetTime = scheduledTime + period;
  m_timerQueue.Push(id, initialTargetTime, period, repeat);
  if (!m_usingRendering) {
    if (IsAnimationFrameRequest(period, repeat)) {
      StartRendering();
    } else if (initialTargetTime <= m_timerQueue.Front().TargetTime) {
      StartDispatcherTimer();
    }
  }
}

void Timing::createTimer(uint32_t id, double duration, double jsSchedulingTime, bool repeat) noexcept {
  if (m_context) {
    winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
        m_context.Handle(),
        [wkThis = std::weak_ptr(this->shared_from_this()), id, duration, jsSchedulingTime, repeat]() {
          if (auto pThis = wkThis.lock()) {
            pThis->createTimerOnQueue(static_cast<uint32_t>(id), duration, jsSchedulingTime, repeat);
          }
        });
  } else if (auto uiDispatcher = m_uiDispatcher.get()) {
    uiDispatcher.Post([wkThis = std::weak_ptr(this->shared_from_this()), id, duration, jsSchedulingTime, repeat]() {
      if (auto pThis = wkThis.lock()) {
        pThis->createTimerOnQueue(static_cast<uint32_t>(id), duration, jsSchedulingTime, repeat);
      }
    });
  }
}

void Timing::deleteTimerOnQueue(uint32_t id) noexcept {
  m_timerQueue.Remove(id);
  if (m_timerQueue.IsEmpty())
    StopTicks();
}

void Timing::deleteTimer(uint32_t id) noexcept {
  if (m_context) {
    winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
        m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), id]() {
          if (auto pThis = wkThis.lock()) {
            pThis->deleteTimerOnQueue(static_cast<int64_t>(id));
          }
        });
  } else if (auto uiDispatcher = m_uiDispatcher.get()) {
    uiDispatcher.Post([wkThis = std::weak_ptr(this->shared_from_this()), id]() {
      if (auto pThis = wkThis.lock()) {
        pThis->deleteTimerOnQueue(static_cast<int64_t>(id));
      }
    });
  }
}

void Timing::setSendIdleEvents(bool /*sendIdleEvents*/) noexcept {
  // TODO: Implement.
}

} // namespace Microsoft::ReactNative
