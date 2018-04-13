// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.DevSupport
{
    /// <summary>
    /// Interface used by <see cref="IDevSupportManager"/> for requesting React
    /// instance regeneration based on the option that the user selects in the
    /// developer options menu.
    /// </summary>
    public interface IReactInstanceDevCommandsHandler
    {
        /// <summary>
        /// Asynchronously locks <see cref="ReactInstanceManager"/> in order to
        /// serialize context (re)initialization
        /// </summary>
        /// <returns>A task to await the Lock on. Releasing the result releases the lock</returns>
        Task<IDisposable> LockAsync();

        /// <summary>
        /// Action to notify the <see cref="ReactInstanceManager"/> about the
        /// availability of a new JavaScript bundle downloaded from the server.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>A task to await the React context.</returns>
        Task<ReactContext> CreateReactContextFromCachedPackagerBundleAsync(CancellationToken token);

        /// <summary>
        /// Action triggered when the user requests that the application be
        /// reloaded from the initially specified bundle file.
        /// </summary>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>A task to await the React context.</returns>
        Task<ReactContext> CreateReactContextFromBundleAsync(CancellationToken token);

        /// <summary>
        /// Action triggered when the user requests that the application be
        /// reloaded from the JavaScript debugger.
        /// </summary>
        /// <param name="javaScriptExecutorFactory">
        /// The JavaScript executor factory.
        /// </param>
        /// <param name="token">A token to cancel the request.</param>
        /// <returns>A task to await the React context.</returns>
        Task<ReactContext> CreateReactContextWithRemoteDebuggerAsync(
            Func<IJavaScriptExecutor> javaScriptExecutorFactory,
            CancellationToken token);
    }
}
