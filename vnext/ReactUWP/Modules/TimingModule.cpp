// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#undef Check

#include "TimingModule.h"

#include <InstanceManager.h>
#include <Utils/ValueUtils.h>

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#include <unknwnbase.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace facebook::xplat;
using namespace folly;
namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt
using namespace std;

namespace react {
namespace uwp {

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

Timing::Timing(TimingModule *parent) : m_parent(parent) {}

void Timing::Disconnect() {
  m_parent = nullptr;
}

std::weak_ptr<facebook::react::Instance> Timing::getInstance() noexcept {
  if (!m_parent)
    return std::weak_ptr<facebook::react::Instance>();

  return m_parent->getInstance();
}

void Timing::OnRendering(const winrt::IInspectable &, const winrt::IInspectable &args) {
  std::vector<int64_t> readyTimers;
  auto now = winrt::DateTime::clock::now();

  while (!m_timerQueue.IsEmpty() && m_timerQueue.Front().TargetTime < now) {
    // Pop first timer from the queue and add it to list of timers ready to fire
    Timer next = m_timerQueue.Front();
    m_timerQueue.Pop();
    readyTimers.push_back(next.Id);

    // If timer is repeating push it back onto the queue for the next repetition
    if (next.Repeat)
      m_timerQueue.Push(next.Id, now + next.Period, next.Period, true);

    if (m_timerQueue.IsEmpty())
      m_rendering.revoke();
  }

  if (!readyTimers.empty()) {
    if (auto instance = getInstance().lock()) {
      // Package list of Timer Ids to fire in a dynamic array to pass as
      // parameter
      folly::dynamic params = folly::dynamic::array();
      for (size_t i = 0, c = readyTimers.size(); i < c; ++i)
        params.push_back(folly::dynamic(readyTimers[i]));

      instance->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(params));
    } else {
      assert(false && "getInstance().lock failed");
    }
  }
}

void Timing::createTimer(int64_t id, double duration, double jsSchedulingTime, bool repeat) {
  if (duration == 0 && !repeat) {
    if (auto instance = getInstance().lock()) {
      folly::dynamic params = folly::dynamic::array(id);
      instance->callJSFunction("JSTimers", "callTimers", folly::dynamic::array(params));
    } else {
      assert(false && "getInstance().lock failed");
    }

    return;
  }

  if (m_timerQueue.IsEmpty()) {
    m_rendering.revoke();
    m_rendering = winrt::CompositionTarget::Rendering(winrt::auto_revoke, {this, &Timing::OnRendering});
  }

  // Convert double duration in ms to TimeSpan
  // Make sure duration is always larger than 16ms to avoid unnecessary wakeups.
  auto period = TimeSpanFromMs(std::max(duration, 16.0));
  const int64_t msFrom1601to1970 = 11644473600000;
  winrt::DateTime scheduledTime(TimeSpanFromMs(jsSchedulingTime + msFrom1601to1970));
  auto initialTargetTime = scheduledTime + period;

  m_timerQueue.Push(id, initialTargetTime, period, repeat);
}

void Timing::deleteTimer(int64_t id) {
  m_timerQueue.Remove(id);

  if (m_timerQueue.IsEmpty())
    m_rendering.revoke();
}

void Timing::setSendIdleEvents(bool /*sendIdleEvents*/) {
  // TODO: Implement.
}

//
// TimingModule
//
const char *TimingModule::name = "Timing";

TimingModule::TimingModule() : m_timing(std::make_shared<Timing>(this)) {}

TimingModule::~TimingModule() {
  if (m_timing != nullptr)
    m_timing->Disconnect();
}

std::string TimingModule::getName() {
  return name;
}

auto TimingModule::getConstants() -> std::map<std::string, dynamic> {
  return {};
}

auto TimingModule::getMethods() -> std::vector<Method> {
  std::shared_ptr<Timing> timing(m_timing);
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

} // namespace uwp
} // namespace react

namespace facebook {
namespace react {

std::unique_ptr<facebook::xplat::module::CxxModule> CreateTimingModule(
    const std::shared_ptr<facebook::react::MessageQueueThread> &) noexcept {
  return std::make_unique<::react::uwp::TimingModule>();
}

} // namespace react
} // namespace facebook
