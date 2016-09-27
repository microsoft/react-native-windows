namespace ReactNative.Bridge
{
    /// <summary>
    /// Asynchronous disposable resource with disposal state tracking.
    /// </summary>
    public interface IAsyncCancelable : IAsyncDisposable
    {
        /// <summary>
        /// Gets a value that indicates whether the object is disposed.
        /// </summary>
        bool IsDisposed { get; }
    }
}
