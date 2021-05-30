// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestEventService.h"
#include <motifCpp/testCheck.h>
#include <sstream>

namespace ReactNativeIntegrationTests {

using namespace std::literals::chrono_literals;

/*static*/ std::mutex TestEventService::s_mutex;
/*static*/ std::condition_variable TestEventService::s_cv;
/*static*/ std::queue<TestEvent> TestEventService::s_eventQueue;
/*static*/ bool TestEventService::s_hasNewEvent{false};

/*static*/ void TestEventService::Initialize() noexcept {
  auto lock = std::scoped_lock{s_mutex};
  s_eventQueue = {};
}

/*static*/ void TestEventService::LogEvent(std::string_view eventName, JSValue &&value) noexcept {
  auto lock = std::scoped_lock{s_mutex};
  s_eventQueue.push(TestEvent{eventName, std::move(value)});
  s_hasNewEvent = true;
  s_cv.notify_all();
}

/*static*/ void TestEventService::ObserveEvents(winrt::array_view<const TestEvent> expectedEvents) noexcept {
  uint32_t observeEventIndex{0};
  while (observeEventIndex < expectedEvents.size()) {
    auto lock = std::unique_lock{s_mutex};
    if (!s_eventQueue.empty()) {
      TestEvent loggedEvent{std::move(s_eventQueue.front())};
      s_eventQueue.pop();

      // Check the event name and value
      auto const &expectedEvent = expectedEvents[observeEventIndex];
      TestCheckEqual(expectedEvent.EventName, loggedEvent.EventName);
      if (auto d1 = expectedEvent.Value.TryGetDouble(), d2 = loggedEvent.Value.TryGetDouble(); d1 && d2) {
        // Comparison of doubles has special logic because NaN != NaN.
        if (!isnan(*d1) && !isnan(*d2)) {
          TestCheckEqual(*d1, *d2);
        }
      } else if (expectedEvent.Value != loggedEvent.Value) { // Use JSValue strict compare
        std::stringstream os;
        os << "Event index: " << observeEventIndex << '\n'
           << "Expected: " << expectedEvent.Value.ToString() << '\n'
           << "Actual: " << loggedEvent.Value.ToString();
        TestCheckFail("%s", os.str().c_str());
      }

      ++observeEventIndex;
    } else {
      s_cv.wait(lock, [&]() { return s_hasNewEvent; });
      s_hasNewEvent = false;
    }
  }
}

} // namespace ReactNativeIntegrationTests
