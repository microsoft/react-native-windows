// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Abstract context wrapper for the React instance to manage
    /// lifecycle events.
    /// </summary>
    public interface IReactContext 
    {
        /// <summary>
        /// Gets the instance of the <see cref="IJavaScriptModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of JavaScript module.</typeparam>
        /// <returns>The JavaScript module instance.</returns>
        T GetJavaScriptModule<T>()
            where T : IJavaScriptModule, new();

        /// <summary>
        /// Gets the instance of the <see cref="INativeModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of native module.</typeparam>
        /// <returns>The native module instance.</returns>
        T GetNativeModule<T>()
            where T : INativeModule;

        /// <summary>
        /// Adds a lifecycle event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void AddLifecycleEventListener(ILifecycleEventListener listener);

        /// <summary>
        /// Removes a lifecycle event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void RemoveLifecycleEventListener(ILifecycleEventListener listener);

        /// <summary>
        /// Adds a background event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void AddBackgroundEventListener(IBackgroundEventListener listener);

        /// <summary>
        /// Removes a background event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        void RemoveBackgroundEventListener(IBackgroundEventListener listener);

        /// <summary>
        /// Checks if the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the dispatcher queue thread,
        ///  <b>false</b> otherwise.
        /// </returns>
        bool IsOnDispatcherQueueThread();

        /// <summary>
        /// Enqueues an action on the dispatcher queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnDispatcherQueueThread(Action action);

        /// <summary>
        /// Checks if the current thread is on the React instance
        /// JavaScript queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the JavaScript queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        bool IsOnJavaScriptQueueThread();

        /// <summary>
        /// Enqueues an action on the JavaScript queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnJavaScriptQueueThread(Action action);

        /// <summary>
        /// Checks if the current thread is on the React instance native 
        /// modules queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the native modules queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        bool IsOnNativeModulesQueueThread();

        /// <summary>
        /// Enqueues an action on the native modules queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        void RunOnNativeModulesQueueThread(Action action);

        /// <summary>
        /// Passes the exception to the current, if set, otherwise rethrows.
        /// </summary>
        /// <param name="exception"></param>
        void HandleException(Exception exception);
    }
}

