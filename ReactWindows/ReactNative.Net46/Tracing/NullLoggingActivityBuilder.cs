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

        public NullDisposable Create()
        {
            return new NullDisposable();
        }
        
        public class NullDisposable : IDisposable
        {
            public void Dispose()
            {

            }
        }
    }
}