// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reactive.Concurrency;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Factory to create the React queue configuration.
    /// </summary>
    class ReactQueueConfigurationFactory
    {
        private readonly IActionQueueFactory _dispatcherQueueFactory;
        private readonly IActionQueueFactory _javaScriptQueueFactory;
        private readonly IActionQueueFactory _nativeModulesQueueFactory;

        /// <summary>
        /// Instantiates the <see cref="ReactQueueConfigurationFactory"/>.
        /// </summary>
        /// <param name="dispatcherQueueFactory">The dispatcher queue factory.</param>
        /// <param name="javaScriptQueueFactory">The JavaScript queue factory.</param>
        /// <param name="nativeModulesQueueFactory">The native modules queue factory.</param>
        public ReactQueueConfigurationFactory(
            IActionQueueFactory dispatcherQueueFactory,
            IActionQueueFactory javaScriptQueueFactory,
            IActionQueueFactory nativeModulesQueueFactory)
        {
            _dispatcherQueueFactory = dispatcherQueueFactory;
            _javaScriptQueueFactory = javaScriptQueueFactory;
            _nativeModulesQueueFactory = nativeModulesQueueFactory;
        }

        /// <summary>
        /// The default queue configuration factory.
        /// </summary>
        public static ReactQueueConfigurationFactory Default
        {
            get
            {
                return new ReactQueueConfigurationFactory(
                    new ActionQueueFactory(onError => new DispatcherActionQueue(onError, DispatcherHelpers.MainDispatcher)),
                    new ActionQueueFactory(onError => new ActionQueue(onError, NewThreadScheduler.Default)),
                    new ActionQueueFactory(onError => new LimitedConcurrencyActionQueue(onError)));
            }
        }

        /// <summary>
        /// Creates the React queue configuration.
        /// </summary>
        /// <param name="onError">The exception handler.</param>
        /// <returns>The React queue configuration.</returns>
        public ReactQueueConfiguration Create(Action<Exception> onError)
        {
            return new ReactQueueConfiguration(
                _dispatcherQueueFactory.Create(onError),
                _javaScriptQueueFactory.Create(onError),
                _nativeModulesQueueFactory.Create(onError));
        }
    }
}
