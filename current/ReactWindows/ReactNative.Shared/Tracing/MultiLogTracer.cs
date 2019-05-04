using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Utility class that allows multiple tracers to accept logs from RnLog.
    /// </summary>
    /// <remarks>
    /// Note that MultiLogTracer itself can have a VerbosityLevel specified, which will filter before forwarding
    /// to any of the child tracers.
    /// </remarks>
    public class MultiLogTracer : LogTracerBase
    {
        private readonly ICollection<LogTracerBase> _tracers;

        /// <summary>
        /// Accepts the list of LogTracerBase instances to log to.
        /// </summary>
        /// <param name="tracers">The list to log to</param>
        public MultiLogTracer(ICollection<LogTracerBase> tracers)
        {
            this._tracers = tracers;
        }

        /// <summary>
        /// Calls Flush for all child tracers.
        /// </summary>
        public override void Flush()
        {
            foreach (var tracer in _tracers)
            {
                tracer.Flush();
            }
        }

        /// <summary>
        /// Disposes the log tracer.
        /// </summary>
        /// <remarks>
        /// The current implementation defers to the list of log tracers
        /// </remarks>
        public override void Dispose()
        {
            foreach (var tracer in _tracers)
            {
                tracer.Dispose();
            }
            _tracers.Clear();
        }

        /// <inheritdoc />
        protected override void AppendInternal(EventLevel eventLevel, string eventName, FormattableString message, Exception exception)
        {
            foreach (var tracer in _tracers)
            {
                tracer.Append(eventLevel, eventName, message, exception);
            }
        }
    }
}
