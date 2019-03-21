using System;
using System.Diagnostics.Tracing;

namespace ReactNative.Tracing
{
    // Here are the goals of this API:
    //   - A common logging API that can be used by React Native apps and modules for
    //     helping to diagnose bugs that happen in debug and release builds.
    //   - The implementation should be configurable by the app so the app can log to
    //     ETW, to a file, or to wherever else it might want.
    //   - The API is designed to be similar to the React Native Android logging API
    //     to make it easier for React Native Android module authors to write React
    //     Native Windows modules. For reference, here's the React Native Android API:
    //     https://github.com/facebook/fresco/blob/8fc70f1699d14706caffbcc7cacb20c7d02f4b9d/fbcore/src/main/java/com/facebook/common/logging/FLog.java
    //   - Each log statement should have several pieces of information associated
    //     with it:
    //     - A string message which the developer can use to log any information
    //       they want.
    //     - A log level which is useful for filtering and understanding the nature
    //       of the message (e.g. info, warn, error).
    //     - A contextual name so we know what generated the message (e.g. a class
    //       or module name).
    //
    // Comparison with other logging APIs
    //   Windows comes with several logging APIs that are available to C#:
    //     - System.Diagnostics.Debug
    //     - System.Diagnostics.Trace
    //     - System.Diagnostics.Tracing
    //     - Windows.Foundation.Diagnostics.LoggingChannel
    //
    //   None of these APIs forces you to associate a contextual name (e.g.
    //   a class or module name) with your log statement to help track who
    //   generated the message. Consequently, we've decided to build an API
    //   on top of the Windows logging APIs rather than recommending to use
    //   the Windows logging APIs directly.
    //
    //   React Native Windows has another logging API called ReactNative.Tracing.Tracer.
    //   Tracer is different in that it's a non-public API which is focused on
    //   logging to understand the performance of React Native Windows internals. Whereas
    //   RnLog is a public API focused on logging for the purposes of diagnosing bugs.

    /// <summary>
    /// A logging API that can be used by React Native apps and modules for helping
    /// to diagnose bugs that happen in debug and release builds.
    /// </summary>
    public static class RnLog
    {
        /// <summary>
        /// By default use DebugLogTracer
        /// </summary>
        private static LogTracerBase logTracer = DebugLogTracer.Instance;

        /// <summary>
        /// Sets log tracer to be used to log a message. Allows only one tracer to be used at the time. By default DebugLogTracer will be used.
        /// This wasn't implemented as a thread safe because of:
        /// - ILogTracer shouldn't be changed a lot of times, in general it should be one time action when app starts
        /// - Any thread safety issues should be addressed in ILogTracer to avoid performance penalty of doing it on that high level
        /// </summary>
        /// <param name="tracer">Log tracer object</param>
        public static void SetLogTracer(LogTracerBase tracer)
        {
            logTracer = tracer;
        }

        private static void WriteLogLine(EventLevel eventLevel, string tag, FormattableString message, Exception exception = null)
        {
            if (logTracer != null)
            {
                logTracer.Append(eventLevel, tag, message, exception);
            }
        }

        /// <summary>
        /// Log an informational message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="message">Message format to log.</param>
        public static void Info(string tag, FormattableString message)
        {
            Info(tag, null, message);
        }

        /// <summary>
        /// Log an informational message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="exception">Exception to include with log (may be null).</param>
        /// <param name="message">Message format to log.</param>
        public static void Info(string tag, Exception exception, FormattableString message)
        {
            WriteLogLine(EventLevel.Informational, tag, message, exception);
        }

        /// <summary>
        /// Log a warning message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="message">The message to log.</param>
        public static void Warn(string tag, FormattableString message)
        {
            Warn(tag, null, message);
        }

        /// <summary>
        /// Log a warning message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="exception">Exception to include with log (may be null).</param>
        /// <param name="message">The message to log.</param>
        public static void Warn(string tag, Exception exception, FormattableString message)
        {
            WriteLogLine(EventLevel.Warning, tag, message, exception);
        }

        /// <summary>
        /// Log an error message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="message">The message to log.</param>
        public static void Error(string tag, FormattableString message)
        {
            Error(tag, null, message);
        }

        /// <summary>
        /// Log an error message.
        /// </summary>
        /// <param name="tag">Identifies the source of the log message. Usually a class or React Native module name.</param>
        /// <param name="exception">Exception to include with log (may be null).</param>
        /// <param name="message">The message to log.</param>
        public static void Error(string tag, Exception exception, FormattableString message)
        {
            WriteLogLine(EventLevel.Error, tag, message, exception);
        }

        /// <summary>
        /// Log an error message and crash the application.
        /// </summary>
        /// <param name="tag">Identifies the source of the crash. Usually a class or React Native module name.</param>
        /// <param name="message">The message to log/include in crash report.</param>
        public static void Fatal(string tag, FormattableString message)
        {
            Fatal(tag, null, message);
        }

        /// <summary>
        /// Log an error message and crash the application.
        /// </summary>
        /// <param name="tag">Identifies the source of the crash. Usually a class or React Native module name.</param>
        /// <param name="exception">Exception to include with crash report (may be null).</param>
        /// <param name="message">The message to log/include in crash report.</param>
        public static void Fatal(string tag, Exception exception, FormattableString message)
        {
            WriteLogLine(EventLevel.Critical, tag, message, exception);
            Flush();
            logTracer.CrashApplication(message, exception);
        }

        /// <summary>
        /// Synchronously flush the logTracer so that any logs in memory are written to their final destination.
        /// </summary>
        public static void Flush()
        {
            logTracer.Flush();
        }
    }
}
