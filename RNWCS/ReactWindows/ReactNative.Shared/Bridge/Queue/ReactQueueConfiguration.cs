// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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
        private readonly IActionQueue _dispatcherQueue;
        private readonly IActionQueue _nativeModulesQueue;
        private readonly IActionQueue _javaScriptQueue;

        /// <summary>
        /// Instantiates the queue configuration.
        /// </summary>
        /// <param name="dispatcherQueue"></param>
        /// <param name="javaScriptQueue"></param>
        /// <param name="nativeModulesQueue"></param>
        public ReactQueueConfiguration(
            IActionQueue dispatcherQueue,
            IActionQueue javaScriptQueue,
            IActionQueue nativeModulesQueue)
        {
            _dispatcherQueue = dispatcherQueue;
            _nativeModulesQueue = nativeModulesQueue;
            _javaScriptQueue = javaScriptQueue;
        }

        /// <summary>
        /// The main UI thread.
        /// </summary>
        public IActionQueue DispatcherQueue
        {
            get
            {
                return _dispatcherQueue;
            }
        }

        /// <summary>
        /// The JavaScript thread.
        /// </summary>
        public IActionQueue JavaScriptQueue
        {
            get
            {
                return _javaScriptQueue;
            }
        }

        /// <summary>
        /// The native modules thread.
        /// </summary>
        public IActionQueue NativeModulesQueue
        {
            get
            {
                return _nativeModulesQueue;
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
            _dispatcherQueue.Dispose();
            _nativeModulesQueue.Dispose();
            _javaScriptQueue.Dispose();
        }
    }
}
