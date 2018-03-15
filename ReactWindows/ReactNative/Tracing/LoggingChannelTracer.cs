// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Foundation.Diagnostics;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Tracing helpers for the application.
    /// </summary>
    class LoggingChannelTracer : ITracer
    {
        /// <summary>
        /// The logging channel instance.
        /// </summary>
        public static LoggingChannel Instance { get; } = new LoggingChannel("ReactWindows", null);

        /// <summary>
        /// Create a logging activity builder.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="name">The event name.</param>
        /// <returns>The logging activity builder.</returns>
        public ITraceBuilder Trace(int tag, string name)
        {
            if (Instance.Enabled)
            {
                return new LoggingActivityBuilder(Instance, name, LoggingLevel.Information, new LoggingOptions
                {
                    Tags = tag,
                });
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// Write an event.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="eventName">The event name.</param>
        public void Write(int tag, string eventName)
        {
            if (Instance.Enabled)
            {
                Instance.LogEvent(eventName, null, LoggingLevel.Information, new LoggingOptions
                {
                    Tags = tag
                });
            }
        }

        /// <summary>
        /// Write an error event.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="eventName">The event name.</param>
        /// <param name="ex">The exception.</param>
        public void Error(int tag, string eventName, Exception ex)
        {
            if (Instance.Enabled)
            {
                Instance.LogEvent(eventName, null, LoggingLevel.Error);
            }
        }
    }
}
