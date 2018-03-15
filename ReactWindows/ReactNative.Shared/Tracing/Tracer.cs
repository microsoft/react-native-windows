// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.PlatformServices;
using System;

namespace ReactNative.Tracing
{
    static class Tracer
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
        /// Trace the duration of an operation.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        /// <returns>A builder to attach tracing properties.</returns>
        public static ITraceBuilder Trace(int tag, string name)
        {
            return ServiceResolver.GetService<ITracer>()?.Trace(tag, name);
        }

        /// <summary>
        /// Traces an error.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        /// <param name="exception">The exception.</param>
        public static void Error(int tag, string name, Exception exception)
        {
            ServiceResolver.GetService<ITracer>()?.Error(tag, name, exception);
        }

        /// <summary>
        /// Traces an event.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        public static void Write(int tag, string name)
        {
            ServiceResolver.GetService<ITracer>()?.Write(tag, name);
        }
    }
}
