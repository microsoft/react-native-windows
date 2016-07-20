using System;
using Windows.Foundation.Diagnostics;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Tracing helpers for the application.
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

        /// <summary>
        /// Write an event.
        /// </summary>
        /// <param name="tag">The trace tag.</param>
        /// <param name="eventName">The event name.</param>
        public static void Write(int tag, string eventName)
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
        public static void Error(int tag, string eventName, Exception ex)
        {
            if (Instance.Enabled)
            {
                Instance.LogEvent(eventName, null, LoggingLevel.Error);
            }
        }
    }
}
