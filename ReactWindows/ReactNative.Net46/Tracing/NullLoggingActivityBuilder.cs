using System;

namespace ReactNative.Tracing
{
    /// <summary>
    /// Temporary NullLoggingActivityBuilder.
    /// </summary>
    public class NullLoggingActivityBuilder
    {
        /// <summary>
        /// Dummy method to satisfy interface requirements.
        /// </summary>
        /// <returns>An empty disposable object.</returns>
        public IDisposable Start()
        {
            return new NullDisposable();
        }

        /// <summary>
        /// Empty disposable object.
        /// </summary>
        class NullDisposable : IDisposable
        {
            /// <summary>
            /// Dispose method to conform to IDisposable.
            /// </summary>
            public void Dispose()
            {

            }
        }
    }
}