using System;

namespace ReactNative.Tracing
{
    /// <summary>
    /// An interface for adding properties to a trace operation.
    /// </summary>
    public partial interface ITraceBuilder
    {
        /// <summary>
        /// Starts tracing an operation.
        /// </summary>
        /// <returns>
        /// A disposable handle to stop the trace.
        /// </returns>
        IDisposable Start();
    }
}
