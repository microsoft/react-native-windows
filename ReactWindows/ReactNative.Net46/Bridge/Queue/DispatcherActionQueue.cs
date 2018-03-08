// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive.Concurrency;
using System.Windows.Threading;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Action queue that runs on a dispatcher thread.
    /// </summary>
    public class DispatcherActionQueue : ActionQueue
    {
        /// <summary>
        /// Instantiates the dispatcher action queue.
        /// </summary>
        /// <param name="onError">The exception handler.</param>
        public DispatcherActionQueue(Action<Exception> onError)
            : this(onError, Dispatcher.CurrentDispatcher)
        {
        }

        /// <summary>
        /// Instantiates the dispatcher action queue.
        /// </summary>
        /// <param name="onError">The exception handler.</param>
        /// <param name="dispatcher">The dispatcher.</param>
        public DispatcherActionQueue(Action<Exception> onError, Dispatcher dispatcher)
            : base(onError, new DispatcherScheduler(dispatcher))
        {
        }
    }
}