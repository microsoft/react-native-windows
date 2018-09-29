using System;
using System.Diagnostics.Tracing;
using System.Globalization;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Logs events as Etw events to SkypeEventSource.
    /// </summary>
    public class EtwLogTracer : LogTracerBase
    {
        private EventSource eventSource;

        /// <summary>
        /// Accepts the source name used for the EventSource.
        /// </summary>
        /// <param name="eventSourceName">The source name</param>
        public EtwLogTracer(string eventSourceName)
        {
            // For EtwSelfDescribingEventFormat tracelogging the guid is automatically computed from the name.
            eventSource = new EventSource(eventSourceName, EventSourceSettings.EtwSelfDescribingEventFormat);
        }

        /// <summary>
        /// Logs an event to the SkypeEventSource ETW
        /// </summary>
        /// <param name="eventLevel">Level of the event</param>
        /// <param name="eventName">Name of the event</param>
        /// <param name="message">Log message</param>
        /// <param name="exception">Optional exception to include with event</param>
        protected override void AppendInternal(EventLevel eventLevel, string eventName, FormattableString message, Exception exception)
        {
            var options = new EventSourceOptions
            {
                Opcode = EventOpcode.Info,
                Keywords = EventKeywords.None,
                Level = eventLevel
            };

            eventSource.Write(
                eventName,
                options,
                new
                {
                    message,
                    timestamp = DateTime.Now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss.fffzzz", CultureInfo.InvariantCulture),
                    exception = FormatException(exception)
                });
        }

        /// <summary>
        /// Disposes the log tracer.
        /// </summary>
        /// <remarks>
        /// The current implementation disposes the event source
        /// </remarks>
        public override void Dispose()
        {
            eventSource.Dispose();
            eventSource = null;
        }
    }
}
