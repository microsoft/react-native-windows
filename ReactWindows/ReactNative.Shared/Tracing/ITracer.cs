using System;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Interface for React Native tracing.
    /// </summary>
    public interface ITracer
    {
        /// <summary>
        /// Trace the duration of an operation.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        /// <returns>A builder to attach tracing properties.</returns>
        ITraceBuilder Trace(int tag, string name);

        /// <summary>
        /// Traces an error.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        /// <param name="exception">The exception.</param>
        void Error(int tag, string name, Exception exception);

        /// <summary>
        /// Traces an event.
        /// </summary>
        /// <param name="tag">The tag.</param>
        /// <param name="name">The event name.</param>
        void Write(int tag, string name);
    }
}
