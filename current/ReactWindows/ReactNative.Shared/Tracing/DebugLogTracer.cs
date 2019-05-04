using System;
using System.Diagnostics;
using System.Diagnostics.Tracing;
using System.Text;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Logs events to the Debug console.
    /// </summary>
    public sealed class DebugLogTracer : LogTracerBase
    {
        private static DebugLogTracer instance = new DebugLogTracer();
        private DebugLogTracer() {
        }

        /// <summary>
        /// The singleton instance of DebugLogTracer.
        /// </summary>
        public static DebugLogTracer Instance
        {
            get => instance;
        }

        /// <inheritdoc />
        protected override void AppendInternal(EventLevel eventLevel, string eventName, FormattableString message, Exception exception)
        {
            StringBuilder toLog = new StringBuilder($"[{eventLevel:g}] [{eventName}] {message}");
            if (exception != null)
            {
                toLog.AppendLine().Append(FormatException(exception));
            }
            Debug.WriteLine(toLog.ToString());
        }
    }
}
