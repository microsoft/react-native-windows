// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestEventService.h"
#include <motifCpp/testCheck.h>
#include <chrono>
#include <sstream>
#include <thread>

namespace ReactNativeIntegrationTests {

using namespace std::literals::chrono_literals;

/*static*/ std::mutex TestEventService::s_mutex;
/*static*/ std::condition_variable TestEventService::s_cv;
/*static*/ TestEvent TestEventService::s_loggedEvent{};
/*static*/ bool TestEventService::s_previousEventIsObserved{true}; // true to allow new event to be logged
/*static*/ uint32_t TestEventService::s_observeEventIndex{0};

/*static*/ void TestEventService::Initialize() noexcept {
  auto lock = std::scoped_lock{s_mutex};
  s_previousEventIsObserved = true;
  s_observeEventIndex = 0;
}

/*static*/ void TestEventService::LogEvent(std::string_view eventName, JSValue &&value) noexcept {
  // Blocks the thread until the previous logged event is observed.
  while (true) {
    {
      // Do not hold the lock while the thread sleeps.
      auto lock = std::scoped_lock{s_mutex};
      if (s_previousEventIsObserved) {
        s_loggedEvent.EventName = eventName;
        s_loggedEvent.Value = std::move(value);
        s_previousEventIsObserved = false;
        s_cv.notify_all();
        break;
      }
    }

    std::this_thread::sleep_for(1ms);
  }
}

/*static*/ void TestEventService::ObserveEvents(winrt::array_view<const TestEvent> expectedEvents) noexcept {
  auto lock = std::unique_lock{s_mutex};
  s_cv.wait(lock, [&]() {
    if (!s_previousEventIsObserved) {
      TestCheck(s_observeEventIndex < expectedEvents.size());
      auto const &expectedEvent = expectedEvents[s_observeEventIndex];

      // Check the event name and value
      TestCheckEqual(expectedEvent.EventName, s_loggedEvent.EventName);
      if (auto d1 = expectedEvent.Value.TryGetDouble(), d2 = s_loggedEvent.Value.TryGetDouble(); d1 && d2) {
        // Comparison of doubles has special logic because NaN != NaN.
        if (!isnan(*d1) && !isnan(*d2)) {
          TestCheckEqual(*d1, *d2);
        }
      } else if (expectedEvent.Value != s_loggedEvent.Value) { // Use JSValue strict compare
        std::stringstream os;
        os << "Event index: " << s_observeEventIndex << '\n'
           << "Expected: " << expectedEvent.Value.ToString() << '\n'
           << "Actual: " << s_loggedEvent.Value.ToString();
        TestCheckFail("%s", os.str().c_str());
      }
      s_previousEventIsObserved = true;
      ++s_observeEventIndex;
    }

    // Finish observing after we observed the last event.
    return s_observeEventIndex >= expectedEvents.size();
  });
}

} // namespace ReactNativeIntegrationTests
