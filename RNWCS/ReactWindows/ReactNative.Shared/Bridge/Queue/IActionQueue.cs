// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// A queue for executing actions in order.
    /// </summary>
    public interface IActionQueue : IDisposable
    {
        /// <summary>
        /// Dispatch an action to the queue.
        /// </summary>
        /// <param name="action">The action.</param>
        /// <remarks>
        /// Returns immediately.
        /// </remarks>
        void Dispatch(Action action);

        /// <summary>
        /// Checks if the current thread is running in the context of this
        /// action queue.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the current thread is running an action
        /// dispatched by this action queue, otherwise <code>false</code>.
        /// </returns>
        bool IsOnThread();
    }
}
