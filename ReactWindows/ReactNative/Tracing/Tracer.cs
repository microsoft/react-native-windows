using System;
using Newtonsoft.Json;
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

        public static LoggingActivityBuilder Trace(int tag, string name)
        {
            return new LoggingActivityBuilder(Instance, name, LoggingLevel.Information, new LoggingOptions
            {
                Tags = tag,
            });
        }

        public static void Write(int tag, string eventName)
        {
            Instance.LogEvent(eventName, null, LoggingLevel.Information, new LoggingOptions
            {
                Tags = tag
            });
        }

        public static void Error(int tag, string eventName, Exception ex)
        {
            Instance.LogEvent(eventName, null, LoggingLevel.Error);
        }
    }
}
