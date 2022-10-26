// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Threading;

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  // Unordered test events
  internal static class TestNotificationService
  {
    private static readonly object s_mutex = new object();
    private static readonly HashSet<string> s_eventSet = new HashSet<string>();

    // Sets the service to the initial state.
    public static void Initialize()
    {
      lock (s_mutex)
      {
        s_eventSet.Clear();
      }
    }

    // Set a new notification.
    public static void Set(string eventName)
    {
      lock (s_mutex)
      {
        s_eventSet.Add(eventName);
        Monitor.PulseAll(s_mutex);
      }
    }

    // Blocks current thread and waits until expected event appears.
    public static void Wait(string eventName)
    {
      lock (s_mutex)
      {
        while (!s_eventSet.Contains(eventName))
        {
          Monitor.Wait(s_mutex);
        }
        s_eventSet.Remove(eventName);
      }
    }
  }
}
