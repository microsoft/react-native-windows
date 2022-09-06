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
#include <queue>
#include <set>
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

// Ordered test notifications
struct TestEventService {
  // Sets the service to the initial state.
  static void Initialize() noexcept;

  // Logs new event in the queue and notifies queue observers to check it.
  static void LogEvent(std::string_view eventName, JSValue &&value) noexcept;

  // Logs new event for value types that need an explicit call to JSValue constructor.
  template <typename TValue>
  static void LogEvent(std::string_view eventName, TValue &&value) noexcept {
    LogEvent(eventName, JSValue{std::forward<TValue>(value)});
  }

  // Blocks current thread and observes all incoming events in the queue until we see them all.
  static void ObserveEvents(winrt::array_view<const TestEvent> expectedEvents) noexcept;

 private:
  static std::mutex s_mutex; // to synchronize access to the fields below
  static std::condition_variable s_cv; // signals about new event
  static std::queue<TestEvent> s_eventQueue; // the event queue
  static bool s_hasNewEvent;
};

// Unordered test events
struct TestNotificationService {
  // Sets the service to the initial state.
  static void Initialize() noexcept;

  // Set a new notification.
  static void Set(std::string_view eventName) noexcept;

  // Blocks current thread and waits until expected event appears.
  static void Wait(std::string_view eventName) noexcept;

 private:
  static std::mutex s_mutex; // to synchronize access to the fields below
  static std::condition_variable s_cv; // signals about new event
  static std::set<std::string, std::less<>> s_events; // a set of events
};

} // namespace ReactNativeIntegrationTests
