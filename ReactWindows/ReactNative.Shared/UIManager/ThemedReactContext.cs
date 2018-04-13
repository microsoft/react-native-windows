// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge;
using System;
using System.Threading.Tasks;

namespace ReactNative.UIManager
{
    /// <summary>
    /// A wrapper <see cref="T:ReactNative.Bridge.ReactContext" /> that delegates lifecycle events to
    /// the original instance of <see cref="T:ReactNative.Bridge.ReactContext" />.
    /// </summary>
    public class ThemedReactContext : IReactContext
    {
        private readonly IReactContext _reactContext;

        /// <summary>
        /// Instantiates the <see cref="ThemedReactContext"/>.
        /// </summary>
        /// <param name="reactContext">The inner context.</param>
        public ThemedReactContext(IReactContext reactContext)
        {
            _reactContext = reactContext ?? throw new ArgumentNullException(nameof(reactContext));
        }

        /// <inheritdoc />
        public T GetNativeModule<T>()
            where T : INativeModule
        {
            return _reactContext.GetNativeModule<T>();
        }

        /// <inheritdoc />
        public T GetJavaScriptModule<T>()
            where T : IJavaScriptModule, new()
        {
            return _reactContext.GetJavaScriptModule<T>();
        }

        /// <inheritdoc />
        public void AddLifecycleEventListener(ILifecycleEventListener listener)
        {
            _reactContext.AddLifecycleEventListener(listener);
        }

        /// <inheritdoc />
        public void RemoveLifecycleEventListener(ILifecycleEventListener listener)
        {
            _reactContext.RemoveLifecycleEventListener(listener);
        }

        /// <inheritdoc />
        public void AddBackgroundEventListener(IBackgroundEventListener listener)
        {
            _reactContext.AddBackgroundEventListener(listener);
        }

        /// <inheritdoc />
        public void RemoveBackgroundEventListener(IBackgroundEventListener listener)
        {
            _reactContext.RemoveBackgroundEventListener(listener);
        }

        /// <inheritdoc />
        public bool IsOnDispatcherQueueThread()
        {
            return _reactContext.IsOnDispatcherQueueThread();
        }

        /// <inheritdoc />
        public void RunOnDispatcherQueueThread(Action action)
        {
            _reactContext.RunOnDispatcherQueueThread(action);
        }

        /// <inheritdoc />
        public bool IsOnJavaScriptQueueThread()
        {
            return _reactContext.IsOnJavaScriptQueueThread();
        }

        /// <inheritdoc />
        public void RunOnJavaScriptQueueThread(Action action)
        {
            _reactContext.RunOnJavaScriptQueueThread(action);
        }

        /// <inheritdoc />
        public bool IsOnNativeModulesQueueThread()
        {
            return _reactContext.IsOnNativeModulesQueueThread();
        }

        /// <inheritdoc />
        public void RunOnNativeModulesQueueThread(Action action)
        {
            _reactContext.RunOnNativeModulesQueueThread(action);
        }

        /// <inheritdoc />
        public void HandleException(Exception exception)
        {
            _reactContext.HandleException(exception);
        }

        /// <inheritdoc />
        public Task DisposeAsync()
        {
            return _reactContext.DisposeAsync();
        }
    }
}
