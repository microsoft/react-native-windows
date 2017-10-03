using System;
using System.Reactive.Concurrency;
using static System.FormattableString;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Specifies which <see cref="IActionQueue"/>s must be used to run
    /// the various contexts of execution within React (dispatcher, native
    /// modules, and JS). Some of these queue *may* be the same but should be
    /// coded against as if they are different.
    /// </summary>
    class ReactQueueConfiguration : IReactQueueConfiguration
    {
        private readonly IActionQueue _dispatcherQueueThread;
        private readonly IActionQueue _layoutQueueThread;
        private readonly IActionQueue _nativeModulesQueueThread;
        private readonly IActionQueue _jsQueueThread;

        private ReactQueueConfiguration(
            IActionQueue dispatcherQueueThread,
            IActionQueue layoutQueueThread,
            IActionQueue nativeModulesQueueThread,
            IActionQueue jsQueueThread)
        {
            _dispatcherQueueThread = dispatcherQueueThread;
            _layoutQueueThread = layoutQueueThread;
            _nativeModulesQueueThread = nativeModulesQueueThread;
            _jsQueueThread = jsQueueThread;
        }

        /// <summary>
        /// The main UI thread.
        /// </summary>
        public IActionQueue DispatcherQueueThread
        {
            get
            {
                return _dispatcherQueueThread;
            }
        }

        /// <summary>
        /// The layout queue thread.
        /// </summary>
        public IActionQueue LayoutQueueThread
        {
            get
            {
                return _layoutQueueThread;
            }
        }

        /// <summary>
        /// The native modules thread.
        /// </summary>
        public IActionQueue NativeModulesQueueThread
        {
            get
            {
                return _nativeModulesQueueThread;
            }
        }

        /// <summary>
        /// The JavaScript thread.
        /// </summary>
        public IActionQueue JavaScriptQueueThread
        {
            get
            {
                return _jsQueueThread;
            }
        }

        /// <summary>
        /// Disposes the queue configuration.
        /// </summary>
        /// <remarks>
        /// Should be called whenever the corresponding <see cref="IReactInstance"/>
        /// is disposed.
        /// </remarks>
        public void Dispose()
        {
            _dispatcherQueueThread.Dispose();
            _layoutQueueThread.Dispose();
            _nativeModulesQueueThread.Dispose();
            _jsQueueThread.Dispose();
        }

        /// <summary>
        /// Factory for the configuration.
        /// </summary>
        /// <param name="spec">The configuration specification.</param>
        /// <param name="exceptionHandler">The exception handler.</param>
        /// <returns>The queue configuration.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "ReactQueueConfiguration is disposable wrapper.")]
        public static ReactQueueConfiguration Create(
            ReactQueueConfigurationSpec spec,
            Action<Exception> exceptionHandler)
        {
            var dispatcherThreadSpec = ActionQueueSpec.DispatcherThreadSpec;
            var dispatcherThread = Create(dispatcherThreadSpec, exceptionHandler);

            var layoutThreadSpec = ActionQueueSpec.LayoutThreadSpec;
            var layoutThread = Create(layoutThreadSpec, exceptionHandler);

            var jsThread = spec.JSQueueThreadSpec != dispatcherThreadSpec
                ? Create(spec.JSQueueThreadSpec, exceptionHandler)
                : dispatcherThread;

            var nativeModulesThread = spec.NativeModulesQueueThreadSpec != dispatcherThreadSpec
                ? Create(spec.NativeModulesQueueThreadSpec, exceptionHandler)
                : dispatcherThread;

            return new ReactQueueConfiguration(
                dispatcherThread,
                layoutThread,
                nativeModulesThread,
                jsThread);
        }

        private static IActionQueue Create(ActionQueueSpec spec, Action<Exception> onError)
        {
            switch (spec.Kind)
            {
                case ActionQueueKind.Dispatcher:
                    return new DispatcherActionQueue(onError);
                case ActionQueueKind.Layout:
                    return new LayoutActionQueue(onError);
                case ActionQueueKind.NewThread:
                    return new ActionQueue(onError, NewThreadScheduler.Default);
                case ActionQueueKind.TaskPool:
                    return new ActionQueue(onError);
                default:
                    throw new InvalidOperationException(
                        Invariant($"Unknown thread type '{spec.Kind}' with name '{spec.Name}'."));
            }
        }
    }
}
