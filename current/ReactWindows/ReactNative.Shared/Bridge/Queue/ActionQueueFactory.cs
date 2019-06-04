// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Anonymous action queue factory implementation.
    /// </summary>
    class ActionQueueFactory : IActionQueueFactory
    {
        private readonly Func<Action<Exception>, IActionQueue> _factory;

        /// <summary>
        /// Instantiates the <see cref="ActionQueueFactory"/>.
        /// </summary>
        /// <param name="factory">The factory implementation.</param>
        public ActionQueueFactory(Func<Action<Exception>, IActionQueue> factory)
        {
            _factory = factory;
        }

        /// <summary>
        /// Creates the action queue.
        /// </summary>
        /// <param name="onError">The exception handler.</param>
        /// <returns>The action queue.</returns>
        public IActionQueue Create(Action<Exception> onError)
        {
            return _factory(onError);
        }
    }
}
