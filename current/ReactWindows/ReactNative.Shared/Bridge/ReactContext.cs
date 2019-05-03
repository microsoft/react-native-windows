// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using ReactNative.Bridge.Queue;
using ReactNative.Common;
using ReactNative.Tracing;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Bridge
{
    /// <summary>
    /// Abstract context wrapper for the React instance to manage
    /// lifecycle events.
    /// </summary>
    public class ReactContext : IAsyncDisposable
    {
        private readonly ReaderWriterLockSlim _lifecycleLock = new ReaderWriterLockSlim();
        private readonly ReaderWriterLockSlim _backgroundLock = new ReaderWriterLockSlim();
        private readonly List<ILifecycleEventListener> _lifecycleEventListeners =
            new List<ILifecycleEventListener>();
        private readonly List<IBackgroundEventListener> _backgroundEventListeners =
            new List<IBackgroundEventListener>();

        private IReactInstance _reactInstance;

        /// <summary>
        /// The React instance associated with the context.
        /// </summary>
        public IReactInstance ReactInstance
        {
            get
            {
                AssertReactInstance();
                return _reactInstance;
            }
        }

        /// <summary>
        /// Checks if the context has an active <see cref="IReactInstance"/>.
        /// </summary>
        public bool HasActiveReactInstance
        {
            get
            {
                return _reactInstance != null && !_reactInstance.IsDisposed;
            }
        }

        /// <summary>
        /// The exception handler for native module calls.
        /// </summary>
        public Action<Exception> NativeModuleCallExceptionHandler
        {
            get;
            set;
        }

        /// <summary>
        /// Gets the instance of the <see cref="IJavaScriptModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of JavaScript module.</typeparam>
        /// <returns>The JavaScript module instance.</returns>
        public T GetJavaScriptModule<T>() 
            where T : IJavaScriptModule, new()
        {
            AssertReactInstance();
            return _reactInstance.GetJavaScriptModule<T>();
        }

        /// <summary>
        /// Gets the instance of the <see cref="INativeModule"/> associated
        /// with the <see cref="IReactInstance"/>.
        /// </summary>
        /// <typeparam name="T">Type of native module.</typeparam>
        /// <returns>The native module instance.</returns>
        public T GetNativeModule<T>()
            where T : INativeModule
        {
            AssertReactInstance();
            return _reactInstance.GetNativeModule<T>();
        }

        /// <summary>
        /// Adds a lifecycle event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void AddLifecycleEventListener(ILifecycleEventListener listener)
        {
            _lifecycleLock.EnterWriteLock();
            try
            {
                _lifecycleEventListeners.Add(listener);
            }
            finally
            {
                _lifecycleLock.ExitWriteLock();
            }
        }

        /// <summary>
        /// Removes a lifecycle event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void RemoveLifecycleEventListener(ILifecycleEventListener listener)
        {
            _lifecycleLock.EnterWriteLock();
            try
            {
                _lifecycleEventListeners.Remove(listener);
            }
            finally
            {
                _lifecycleLock.ExitWriteLock();
            }
        }

        /// <summary>
        /// Adds a background event listener to the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void AddBackgroundEventListener(IBackgroundEventListener listener)
        {
            _backgroundLock.EnterWriteLock();
            try
            {
                _backgroundEventListeners.Add(listener);
            }
            finally
            {
                _backgroundLock.ExitWriteLock();
            }
        }

        /// <summary>
        /// Removes a background event listener from the context.
        /// </summary>
        /// <param name="listener">The listener.</param>
        public void RemoveBackgroundEventListener(IBackgroundEventListener listener)
        {
            _backgroundLock.EnterWriteLock();
            try
            {
                _backgroundEventListeners.Remove(listener);
            }
            finally
            {
                _backgroundLock.ExitWriteLock();
            }
        }


        /// <summary>
        /// Called by the host when the application suspends.
        /// </summary>
        public void OnSuspend()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var clone = default(List<ILifecycleEventListener>);

            _lifecycleLock.EnterReadLock();
            try
            {
                clone = _lifecycleEventListeners.ToList(/* clone */);
            }
            finally
            {
                _lifecycleLock.ExitReadLock();
            }

            foreach (var listener in clone)
            {
                try
                {
                    listener.OnSuspend();
                }
                catch (Exception e)
                {
                    HandleException(e);
                }
            }
        }

        /// <summary>
        /// Called by the host when the application resumes.
        /// </summary>
        public void OnResume()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var clone = default(List<ILifecycleEventListener>);

            _lifecycleLock.EnterReadLock();
            try
            {
                clone = _lifecycleEventListeners.ToList(/* clone */);
            }
            finally
            {
                _lifecycleLock.ExitReadLock();
            }

            foreach (var listener in clone)
            {
                try
                {
                    listener.OnResume();
                }
                catch (Exception e)
                {
                    HandleException(e);
                }
            }
        }

        /// <summary>
        /// Called by the host when the application is destroyed.
        /// </summary>
        public void OnDestroy()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var clone = default(List<ILifecycleEventListener>);

            _lifecycleLock.EnterReadLock();
            try
            {
                clone = _lifecycleEventListeners.ToList(/* clone */);
            }
            finally
            {
                _lifecycleLock.ExitReadLock();
            }

            foreach (var listener in clone)
            {
                try
                {
                    listener.OnDestroy();
                }
                catch (Exception e)
                {
                    HandleException(e);
                }
            }
        }

        /// <summary>
        /// Called when the host is entering the background.
        /// </summary>
        public void OnEnteredBackground()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var clone = default(List<IBackgroundEventListener>);

            _backgroundLock.EnterReadLock();
            try
            {
                clone = _backgroundEventListeners.ToList(/* clone */);
            }
            finally
            {
                _backgroundLock.ExitReadLock();
            }

            foreach (var listener in clone)
            {
                try
                {
                    listener.OnEnteredBackground();
                }
                catch (Exception e)
                {
                    HandleException(e);
                }
            }
        }

        /// <summary>
        /// Called when the host is leaving the background.
        /// </summary>
        public void OnLeavingBackground()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var clone = default(List<IBackgroundEventListener>);

            _backgroundLock.EnterReadLock();
            try
            {
                clone = _backgroundEventListeners.ToList(/* clone */);
            }
            finally
            {
                _backgroundLock.ExitReadLock();
            }

            foreach (var listener in clone)
            {
                try
                {
                    listener.OnLeavingBackground();
                }
                catch (Exception e)
                {
                    HandleException(e);
                }
            }
        }

        /// <summary>
        /// Called by the host when the application shuts down.
        /// </summary>
        public async Task DisposeAsync()
        {
            DispatcherHelpers.AssertOnDispatcher();

            var reactInstance = _reactInstance;
            if (reactInstance != null)
            {
                await reactInstance.DisposeAsync().ConfigureAwait(false);
            }

            _lifecycleLock.Dispose();
        }

        /// <summary>
        /// Checks if the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the dispatcher queue thread,
        ///  <b>false</b> otherwise.
        /// </returns>
        public bool IsOnDispatcherQueueThread()
        {
            AssertReactInstance();
            return _reactInstance.QueueConfiguration.DispatcherQueue.IsOnThread();
        }

        /// <summary>
        /// Asserts that the current thread is on the React instance dispatcher
        /// queue thread.
        /// </summary>
        public void AssertOnDispatcherQueueThread()
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.DispatcherQueue.AssertOnThread();
        }

        /// <summary>
        /// Enqueues an action on the dispatcher queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        public void RunOnDispatcherQueueThread(Action action)
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.DispatcherQueue.Dispatch(action);
        }

        /// <summary>
        /// Checks if the current thread is on the React instance
        /// JavaScript queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the JavaScript queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        public bool IsOnJavaScriptQueueThread()
        {
            AssertReactInstance();
            return _reactInstance.QueueConfiguration.JavaScriptQueue.IsOnThread();
        }

        /// <summary>
        /// Asserts that the current thread is on the React instance
        /// JavaScript queue thread.
        /// </summary>
        public void AssertOnJavaScriptQueueThread()
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.JavaScriptQueue.AssertOnThread();
        }

        /// <summary>
        /// Enqueues an action on the JavaScript queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        public void RunOnJavaScriptQueueThread(Action action)
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.JavaScriptQueue.Dispatch(action);
        }

        /// <summary>
        /// Checks if the current thread is on the React instance native 
        /// modules queue thread.
        /// </summary>
        /// <returns>
        /// <b>true</b> if the call is from the native modules queue thread,
        /// <b>false</b> otherwise.
        /// </returns>
        public bool IsOnNativeModulesQueueThread()
        {
            AssertReactInstance();
            return _reactInstance.QueueConfiguration.NativeModulesQueue.IsOnThread();
        }

        /// <summary>
        /// Asserts that the current thread is on the React instance native
        /// modules queue thread.
        /// </summary>
        public void AssertOnNativeModulesQueueThread()
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.NativeModulesQueue.AssertOnThread();
        }

        /// <summary>
        /// Enqueues an action on the native modules queue thread.
        /// </summary>
        /// <param name="action">The action.</param>
        public void RunOnNativeModulesQueueThread(Action action)
        {
            AssertReactInstance();
            _reactInstance.QueueConfiguration.NativeModulesQueue.Dispatch(action);
        }

        /// <summary>
        /// Passes the exception to the current 
        /// <see cref="NativeModuleCallExceptionHandler"/>, if set, otherwise
        /// rethrows.
        /// </summary>
        /// <param name="exception"></param>
        public void HandleException(Exception exception)
        {
            var nativeModuleCallExceptionHandler = NativeModuleCallExceptionHandler;
            if (_reactInstance != null &&
                !_reactInstance.IsDisposed &&
                nativeModuleCallExceptionHandler != null)
            {
                nativeModuleCallExceptionHandler(exception);
            }
            else
            {
                RnLog.Fatal(ReactConstants.RNW, exception, $"Unhandled Exception in React Context");
            }
        }

        /// <summary>
        /// Set and initialize the <see cref="IReactInstance"/> instance
        /// for this context.
        /// </summary>
        /// <param name="instance">The React instance.</param>
        /// <remarks>
        /// This method should be called exactly once.
        /// </remarks>
        internal void InitializeWithInstance(IReactInstance instance)
        {
            if (instance == null)
                throw new ArgumentNullException(nameof(instance));

            if (_reactInstance != null)
            {
                throw new InvalidOperationException("React instance has already been set.");
            }

            _reactInstance = instance;
        }

        private void AssertReactInstance()
        {
            if (_reactInstance == null)
            {
                throw new InvalidOperationException("React instance has not been set.");
            }
        }
    }
}
