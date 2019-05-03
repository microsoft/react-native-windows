// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

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
        /// The main UI queue.
        /// </summary>
        IActionQueue DispatcherQueue { get; }

        /// <summary>
        /// The native modules queue.
        /// </summary>
        IActionQueue NativeModulesQueue { get; }

        /// <summary>
        /// The JavaScript queue.
        /// </summary>
        IActionQueue JavaScriptQueue { get; }
    }
}
