using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Specifies which <see cref="IActionQueue"/>s must be used to run
    /// the various contexts of execution within React (dispatcher, native
    /// modules, and JS). Some of these queue *may* be the same but should be
    /// coded against as if they are different.
    /// </summary>
    public interface IReactQueueConfiguration : IDisposable
    {
        /// <summary>
        /// The main UI thread.
        /// </summary>
        IActionQueue DispatcherQueueThread { get; }

        /// <summary>
        /// The layout thread.
        /// </summary>
        IActionQueue LayoutQueueThread { get; }

        /// <summary>
        /// The native modules thread.
        /// </summary>
        IActionQueue NativeModulesQueueThread { get; }

        /// <summary>
        /// The JavaScript thread.
        /// </summary>
        IActionQueue JavaScriptQueueThread { get; }
    }
}
