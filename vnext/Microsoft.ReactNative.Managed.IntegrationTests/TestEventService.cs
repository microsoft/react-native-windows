// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System.Threading;

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  internal struct TestEvent
  {
    public string EventName;
    public JSValue Value;

    public TestEvent(string eventName, JSValue value = default)
    {
      EventName = eventName;
      Value = value;
    }
  }

  // Ordered test notifications
  internal static class TestEventService
  {
    private static readonly object s_mutex = new object();
    private static readonly Queue<TestEvent> s_eventQueue = new Queue<TestEvent>();

    // Sets the service to the initial state.
    public static void Initialize()
    {
      lock (s_mutex)
      {
        s_eventQueue.Clear();
      }
    }

    // Logs new event in the queue and notifies queue observers to check it.
    public static void LogEvent(string eventName, JSValue value = default)
    {
      lock (s_mutex)
      {
        s_eventQueue.Enqueue(new TestEvent { EventName = eventName, Value = value });
        Monitor.PulseAll(s_mutex);
      }
    }

    // Blocks current thread and observes all incoming events in the queue until we see them all.
    public static void ObserveEvents(IList<TestEvent> expectedEvents)
    {
      lock (s_mutex)
      {
        int observeEventIndex = 0;
        while (observeEventIndex < expectedEvents.Count)
        {
          if (s_eventQueue.TryDequeue(out TestEvent loggedEvent))
          {
            // Check the event name and value
            var expectedEvent = expectedEvents[observeEventIndex];
            Assert.AreEqual(expectedEvent.EventName, loggedEvent.EventName);

            if (expectedEvent.Value.TryGetDouble(out double d1) && loggedEvent.Value.TryGetDouble(out double d2))
            {
              // Comparison of doubles has special logic because NaN != NaN.
              Assert.AreEqual(double.IsNaN(d1), double.IsNaN(d2));
              if (!double.IsNaN(d1) && !double.IsNaN(d2))
              {
                Assert.AreEqual(d1, d2);
              }
            }
            else
            {
              Assert.AreEqual(expectedEvent.Value, loggedEvent.Value);
            }

            ++observeEventIndex;
          }
          else
          {
            Monitor.Wait(s_mutex);
          }
        }
      }
    }
  }
}
