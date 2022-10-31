// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#undef Check

#include "TimingModule.h"

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

//
// TimerQueue
//

TimerQueue::TimerQueue() {
  std::make_heap(m_timerVector.begin(), m_timerVector.end());
}

void TimerQueue::Push(int64_t id, TDateTime targetTime, TTimeSpan period, bool repeat) {
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

void TimerQueue::Remove(int64_t id) {
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

//
// Timing
//

void Timing::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_usePostForRendering = !xaml::TryGetCurrentApplication();
}

void Timing::OnTick() {
  winrt::Microsoft::ReactNative::JSValueArray readyTimers;
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
    m_context.CallJSFunction(
        L"JSTimers", L"callTimers", winrt::Microsoft::ReactNative::JSValueArray{std::move(readyTimers)});
  }
}

winrt::dispatching::DispatcherQueueTimer Timing::EnsureDispatcherTimer() {
  if (!m_dispatcherQueueTimer) {
    const auto queue = winrt::dispatching::DispatcherQueue::GetForCurrentThread();
    m_dispatcherQueueTimer = queue.CreateTimer();
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
  m_context.UIDispatcher().Post([wkThis = std::weak_ptr(this->shared_from_this())]() {
    if (auto pThis = wkThis.lock()) {
      pThis->m_usingRendering = false;
      pThis->OnTick();
    }
  });
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

void Timing::createTimerOnQueue(int64_t id, double duration, double jsSchedulingTime, bool repeat) noexcept {
  if (duration == 0 && !repeat) {
    m_context.CallJSFunction(
        L"JSTimers",
        L"callTimers",
        winrt::Microsoft::ReactNative::JSValueArray{winrt::Microsoft::ReactNative::JSValueArray{id}});

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

void Timing::createTimer(double id, double duration, double jsSchedulingTime, bool repeat) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), id, duration, jsSchedulingTime, repeat]() {
        if (auto pThis = wkThis.lock()) {
          pThis->createTimerOnQueue(static_cast<int64_t>(id), duration, jsSchedulingTime, repeat);
        }
      });
}

void Timing::deleteTimerOnQueue(int64_t id) noexcept {
  m_timerQueue.Remove(id);
  if (m_timerQueue.IsEmpty())
    StopTicks();
}

void Timing::deleteTimer(double id) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), id]() {
        if (auto pThis = wkThis.lock()) {
          pThis->deleteTimerOnQueue(static_cast<int64_t>(id));
        }
      });
}

void Timing::setSendIdleEvents(bool /*sendIdleEvents*/) noexcept {
  // TODO: Implement.
}

} // namespace Microsoft::ReactNative
