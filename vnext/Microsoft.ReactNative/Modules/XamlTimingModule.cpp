// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#undef Check

#include "XamlTimingModule.h"

#include <InstanceManager.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>

#include <cxxreact/JsArgumentHelpers.h>

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
// XamlTimerQueue
//

XamlTimerQueue::XamlTimerQueue() {
  std::make_heap(m_timerVector.begin(), m_timerVector.end());
}

void XamlTimerQueue::Push(int64_t id, TDateTime targetTime, TTimeSpan period, bool repeat) {
  m_timerVector.emplace_back(id, targetTime, period, repeat);
  std::push_heap(m_timerVector.begin(), m_timerVector.end());
}

void XamlTimerQueue::Pop() {
  std::pop_heap(m_timerVector.begin(), m_timerVector.end());
  m_timerVector.pop_back();
}

Timer &XamlTimerQueue::Front() {
  return m_timerVector.front();
}

void XamlTimerQueue::Remove(int64_t id) {
  // TODO: This is very inefficient, but doing this with a heap is inherently
  // hard. If performance is not good
  //	enough for the scenarios then a different structure is probably needed.
  auto found = std::find(m_timerVector.begin(), m_timerVector.end(), id);
  if (found != m_timerVector.end())
    m_timerVector.erase(found);

  std::make_heap(m_timerVector.begin(), m_timerVector.end());
}

bool XamlTimerQueue::IsEmpty() {
  return m_timerVector.empty();
}

//
// XamlTiming
//

XamlTiming::XamlTiming(XamlTimingModule *parent) : m_parent(parent) {}

void XamlTiming::Disconnect() {
  m_parent = nullptr;
  StopTicks();
}

std::weak_ptr<facebook::react::Instance> XamlTiming::getInstance() noexcept {
  if (!m_parent)
    return std::weak_ptr<facebook::react::Instance>();

  return m_parent->getInstance();
}

void XamlTiming::OnTick() {
  std::vector<int64_t> readyTimers;
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
  } else if (!m_usingRendering || !emittedAnimationFrame) {
    // If we're using a rendering callback, check if any animation frame
    // requests were emitted in this tick. If not, start the dispatcher timer.
    // This extra frame gives JS a chance to enqueue an additional animation
    // frame request before switching tick schedulers.
    StartDispatcherTimer();
  }

  if (!readyTimers.empty()) {
    if (auto instance = getInstance().lock()) {
      // Package list of Timer Ids to fire in a dynamic array to pass as
      // parameter
      folly::dynamic params = folly::dynamic::array();
      for (size_t i = 0, c = readyTimers.size(); i < c; ++i)
        params.push_back(folly::dynamic(readyTimers[i]));

      instance->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(params));
    }
  }
}

winrt::dispatching::DispatcherQueueTimer XamlTiming::EnsureDispatcherTimer() {
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

void XamlTiming::StartRendering() {
  if (m_dispatcherQueueTimer)
    m_dispatcherQueueTimer.Stop();

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

void XamlTiming::StartDispatcherTimer() {
  const auto &nextTimer = m_timerQueue.Front();
  m_rendering.revoke();
  m_usingRendering = false;
  auto timer = EnsureDispatcherTimer();
  timer.Interval(std::max(nextTimer.TargetTime - TDateTime::clock::now(), TTimeSpan::zero()));
  timer.Start();
}

void XamlTiming::StopTicks() {
  m_rendering.revoke();
  m_usingRendering = false;
  if (m_dispatcherQueueTimer)
    m_dispatcherQueueTimer.Stop();
}

void XamlTiming::createTimer(int64_t id, double duration, double jsSchedulingTime, bool repeat) {
  if (duration == 0 && !repeat) {
    if (auto instance = getInstance().lock()) {
      folly::dynamic params = folly::dynamic::array(id);
      instance->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(params));
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

void XamlTiming::deleteTimer(int64_t id) {
  m_timerQueue.Remove(id);
  if (m_timerQueue.IsEmpty())
    StopTicks();
}

void XamlTiming::setSendIdleEvents(bool /*sendIdleEvents*/) {
  // TODO: Implement.
}

//
// XamlTimingModule
//
const char *XamlTimingModule::name = "Timing";

XamlTimingModule::XamlTimingModule() : m_timing(std::make_shared<XamlTiming>(this)) {}

XamlTimingModule::~XamlTimingModule() {
  if (m_timing != nullptr)
    m_timing->Disconnect();
}

std::string XamlTimingModule::getName() {
  return name;
}

auto XamlTimingModule::getConstants() -> std::map<std::string, dynamic> {
  return {};
}

auto XamlTimingModule::getMethods() -> std::vector<Method> {
  std::shared_ptr<XamlTiming> timing(m_timing);
  return {
      Method(
          "createTimer",
          [timing](dynamic args) // int64_t id, double duration, double
                                 // jsSchedulingTime, bool repeat
          {
            timing->createTimer(
                jsArgAsInt(args, 0), jsArgAsDouble(args, 1), jsArgAsDouble(args, 2), jsArgAsBool(args, 3));
          }),
      Method(
          "deleteTimer",
          [timing](dynamic args) // int64_t code, const std::string& reason,
                                 // int64_t id
          { timing->deleteTimer(jsArgAsInt(args, 0)); }),
      Method(
          "setSendIdleEvents",
          [timing](dynamic args) // const std::string& message, int64_t id
          { timing->setSendIdleEvents(jsArgAsBool(args, 0)); }),
  };
}

std::unique_ptr<facebook::xplat::module::CxxModule> CreateXamlTimingModule(
    const std::shared_ptr<facebook::react::MessageQueueThread> &) noexcept {
  return std::make_unique<Microsoft::ReactNative::XamlTimingModule>();
}

} // namespace Microsoft::ReactNative
