// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Factory to create an action queue.
    /// </summary>
    public interface IActionQueueFactory
    {
        /// <summary>
        /// Creates the action queue.
        /// </summary>
        /// <param name="onError">The exception handler.</param>
        /// <returns>The action queue.</returns>
        IActionQueue Create(Action<Exception> onError);
    }
}
