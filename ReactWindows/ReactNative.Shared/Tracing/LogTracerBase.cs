using System;
using System.Diagnostics.Tracing;
using System.Text;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Generic interface to provide log tracer functionality
    /// Used by <see cref="RnLog"/> class to trace log messages.
    /// The interface is designed to be able to integrate other logging
    /// frameworks like nlog, log4net, etc
    /// </summary>
    public abstract class LogTracerBase: IDisposable
    {
        private EventLevel _verbosityLevel = EventLevel.Verbose;

        /// <summary>
        /// Logs a message if the event level is as verbose or less verbose than the tracer verbosity level.
        /// </summary>
        /// <param name="eventLevel">Event level <see cref="EventLevel"/></param>
        /// <param name="eventName">Name of the event</param>
        /// <param name="message">Log message</param>
        /// <param name="exception">Optional exception to include with event</param>
        public void Append(EventLevel eventLevel, string eventName, FormattableString message, Exception exception = null)
        {
            if (eventLevel <= _verbosityLevel)
            {
                AppendInternal(eventLevel, eventName, message, exception);
            }
        }

        /// <summary>
        /// Flushes any internal buffers to whatever their final destination should be synchronously. Subclasses
        /// that keep internal buffers should override this method - it is a no-op by default.
        /// </summary>
        public virtual void Flush()
        {
            // no op by default
        }

        /// <summary>
        /// Crashes the application.
        /// </summary>
        public virtual void CrashApplication(FormattableString message, Exception exception = null)
        {
            if (exception == null)
            {
                exception = new InvalidOperationException("<null exception>");
            }
            Environment.FailFast(message.ToString(), exception);
        }

        /// <summary>
        /// Logs an event message unconditionally
        /// </summary>
        /// <param name="eventLevel">Event level <see cref="EventLevel"/></param>
        /// <param name="eventName">Name of the event</param>
        /// <param name="message">Log message</param>
        /// <param name="exception">Optional exception to include with event</param>
        protected abstract void AppendInternal(EventLevel eventLevel, string eventName, FormattableString message, Exception exception);

        /// <summary>
        /// Sets the threshold for log events - events more verbose than this will not be logged.
        /// </summary>
        /// <param name="verbosityLevel">The maximally verbose EventLevel to log</param>
        public void SetVerbosityLevel(EventLevel verbosityLevel)
        {
            _verbosityLevel = verbosityLevel;
        }

        /// <summary>
        /// Converts an exception into a string formatted nicely so that it can be logged.
        /// </summary>
        /// <param name="ex">The exception to format into a string.</param>
        /// <returns>The exception represented as a nicely formatted string.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Justification = "This code must not fail in any case since it's not critical.")]
        protected static string FormatException(Exception ex)
        {
            if (ex == null)
            {
                return "<null exception>";
            }
            else if (ex is AggregateException aex)
            {
                try
                {
                    var sb = new StringBuilder();
                    sb.Append("AggregateException. Inner exceptions count=" + aex.InnerExceptions.Count).AppendLine();
                    for (int i = 0; i < aex.InnerExceptions.Count; i++)
                    {
                        sb.Append("Inner exception #" + i).AppendLine().Append(FormatException(aex.InnerExceptions[i])).AppendLine();
                    }
                    return sb.ToString();
                }
                catch (Exception excp)
                {
                    return "Failed to get inner exceptions " + excp;
                }
            }
            else
            {
                return ex.ToString();
            }
        }

        /// <summary>
        /// Disposes the log tracer.
        /// </summary>
        /// <remarks>
        /// The current implementation does a Flush
        /// </remarks>
        public virtual void Dispose()
        {
            Flush();
        }
    }
}
