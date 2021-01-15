// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// The TestEventService allows to verify sequence of test events.
// The events are logged by the TestEventService::LogEvent method.
// They are observed by the TestEventService::ObserveEvents method which blocks current thread.
// Each test must call TestEventService::Initialize to reset the service state.

#include <JSValue.h>
#include <winrt/base.h>
#include <mutex>
#include <string_view>

namespace ReactNativeIntegrationTests {

using namespace winrt::Microsoft::ReactNative;

struct TestEvent {
  TestEvent() = default;
  template <typename TName, typename TValue>
  TestEvent(TName &&name, TValue &&value) : EventName(std::forward<TName>(name)), Value(std::forward<TValue>(value)) {}

  std::string EventName;
  JSValue Value;
};

struct TestEventService {
  // Sets to the service to the initial state.
  static void Initialize() noexcept;

  // Logs new event and notifies the observer to check it.
  // It blocks current thread until the previous event is observed.
  //
  // The expectation is that this method is always called on a thread
  // different to the one that runs the ObserveEvents method.
  // We will have a deadlock if this expectation is not met.
  static void LogEvent(std::string_view eventName, JSValue &&value) noexcept;

  // Logs new event for value types that need an explicit call to JSValue constructor.
  template <typename TValue>
  static void LogEvent(std::string_view eventName, TValue &&value) noexcept {
    LogEvent(eventName, JSValue{std::forward<TValue>(value)});
  }

  // Blocks current thread and observes all incoming events until we see them all.
  static void ObserveEvents(winrt::array_view<const TestEvent> expectedEvents) noexcept;

 private:
  static std::mutex s_mutex; // to synchronize access to the fields below
  static std::condition_variable s_cv; // to notify about new event
  static TestEvent s_loggedEvent; // last logged event
  static bool s_previousEventIsObserved; // did we observe the last logged event?
  static uint32_t s_observeEventIndex; // the event index to observe
};

} // namespace ReactNativeIntegrationTests
