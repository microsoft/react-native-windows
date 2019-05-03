// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Foundation.Diagnostics;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Tracing helpers for understanding the performance of React Native Windows
    /// internals.
    /// </summary>
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
        /// The logging channel instance.
        /// </summary>
        public static LoggingChannel Instance { get; } = new LoggingChannel("ReactWindows", null);

        /// <summary>
        /// Create a logging activity builder.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="name">The event name.</param>
        /// <returns>The logging activity builder.</returns>
        public static LoggingActivityBuilder Trace(int tag, string name)
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
    }
}
