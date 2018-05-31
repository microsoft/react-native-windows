// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Temporary NullTracing helpers for the application.
    /// </summary>
    public static class Tracer
    {
        /// <summary>
        /// Trace ID for bridge events.
        /// </summary>
        public const int TRACE_TAG_REACT_BRIDGE = 0;

        /// <summary>
        /// Trace ID for application events.
        /// </summary>
        public const int TRACE_TAG_REACT_APPS = 1;

        /// <summary>
        /// Trace ID for view events.
        /// </summary>
        public const int TRACE_TAG_REACT_VIEW = 2;

        /// <summary>
        /// Create a null logging activity builder.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="name">The event name.</param>
        /// <returns>The null logging activity builder with a fake Start method.</returns>
        public static NullLoggingActivityBuilder Trace(int tag, string name)
        {
            return new NullLoggingActivityBuilder();
        }

        /// <summary>
        /// Write an event.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="eventName">The event name.</param>
        public static void Write(int tag, string eventName)
        {
        }

        /// <summary>
        /// Write an error event.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="eventName">The event name.</param>
        /// <param name="ex">The exception.</param>
        public static void Error(int tag, string eventName, Exception ex)
        {
        }
    }
}