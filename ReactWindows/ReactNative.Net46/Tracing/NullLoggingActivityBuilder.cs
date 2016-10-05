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
        
        class NullDisposable : IDisposable
        {
            public void Dispose()
            {

            }
        }
    }
}