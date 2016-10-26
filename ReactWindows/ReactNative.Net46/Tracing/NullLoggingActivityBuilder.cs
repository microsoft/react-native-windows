using System;
using System.Reactive.Disposables;

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
            return Disposable.Empty;
        }

        public IDisposable Create()
        {
            return Disposable.Empty;
        }
    }
}