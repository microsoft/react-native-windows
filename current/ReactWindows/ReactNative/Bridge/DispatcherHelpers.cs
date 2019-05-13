// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative.Common;
using ReactNative.Tracing;
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A set of helpers for dispatcher access.
    /// </summary>
    public static class DispatcherHelpers
    {
        private static ThreadLocal<bool> s_isOnDispatcherThread;
        private static CoreDispatcher s_mainDispatcher;

        /// <summary>
        /// Gets the main dispatcher for the app.
        /// </summary>
        public static CoreDispatcher MainDispatcher
        {
            get
            {
                if (s_mainDispatcher == null)
                {
                    throw new InvalidOperationException("Main dispatcher has not been set, please run Initialize.");
                }

                return s_mainDispatcher;
            }
            private set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                s_mainDispatcher = value;
            }
        }

        /// <summary>
        /// Sets the main dispatcher.
        /// </summary>
        public static void Initialize()
        {
            s_mainDispatcher = CoreApplication.MainView.Dispatcher;
        }

        /// <summary>
        /// Asserts that the current thread has dispatcher access.
        /// </summary>
        public static void AssertOnDispatcher()
        {
#if DEBUG
            if (!IsOnDispatcher())
            {
                throw new InvalidOperationException("Thread does not have main dispatcher access.");
            }
#endif
        }

        /// <summary>
        /// Checks if the current thread has dispatcher access.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the current thread has main dispatcher access,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsOnDispatcher()
        {
            if (s_isOnDispatcherThread == null)
            {
                if (MainDispatcher.HasThreadAccess)
                {
                    s_isOnDispatcherThread = new ThreadLocal<bool>
                    {
                        Value = true,
                    };

                    return true;
                }
                else
                {
                    return false;
                }
            }

            return s_isOnDispatcherThread.Value;
        }

        /// <summary>
        /// Asserts that the current thread has access to a DependencyObject.
        /// </summary>
        public static void AssertOnDispatcher(DependencyObject dependencyObject)
        {
#if DEBUG
            if (!IsOnDispatcher(dependencyObject))
            {
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
#endif
        }

        /// <summary>
        /// Checks if the current thread has access to a DependencyObject.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the current thread has dispatcher access,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsOnDispatcher(DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
            {
                throw new ArgumentNullException("dependencyObject");
            }

            return IsOnDispatcher(dependencyObject.Dispatcher);
        }

        /// <summary>
        /// Checks if the current thread has access to a DependencyObject.
        /// </summary>
        /// <returns>
        /// <code>true</code> if the current thread has dispatcher access,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsOnDispatcher(CoreDispatcher dispatcher)
        {
            if (dispatcher == null)
            {
                throw new ArgumentNullException("dispatcher");
            }

            // Fast path for main dispatcher
            if (dispatcher == s_mainDispatcher)
            {
                return IsOnDispatcher();
            }

            return CoreApplication.GetCurrentView()?.Dispatcher == dispatcher;
        }

        /// <summary>
        /// Invokes an action on the main dispatcher.
        /// </summary>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the main dispatcher.</param>
        public static void RunOnDispatcher(DispatchedHandler action, bool allowInlining = false)
        {
            RunOnDispatcher(CoreDispatcherPriority.Normal, action, allowInlining);
        }

        /// <summary>
        /// Invokes an action on the main dispatcher with custom priority.
        /// </summary>
        /// <param name="priority">The priority.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the main dispatcher.</param>
        public static void RunOnDispatcher(CoreDispatcherPriority priority, DispatchedHandler action, bool allowInlining = false)
        {
            RunOnDispatcher(MainDispatcher, priority, action, allowInlining);
        }

        /// <summary>
        /// Invokes an action on a specified dispatcher.
        /// </summary>
        /// <param name="dispatcher">The dispatcher.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        public static void RunOnDispatcher(CoreDispatcher dispatcher, DispatchedHandler action, bool allowInlining = false)
        {
            RunOnDispatcher(dispatcher, CoreDispatcherPriority.Normal, action, allowInlining);
        }

        /// <summary>
        /// Invokes an action on a specified dispatcher and priority
        /// </summary>
        /// <param name="dispatcher">The dispatcher.</param>
        /// <param name="priority">The priority.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        public static void RunOnDispatcher(CoreDispatcher dispatcher, CoreDispatcherPriority priority, DispatchedHandler action, bool allowInlining = false)
        {
            if (allowInlining && IsOnDispatcher(dispatcher))
            {
                action();
            }
            else
            {
                dispatcher.RunAsync(priority, action).AsTask().ContinueWith(
                    t =>
                    {
                        RnLog.Fatal(ReactConstants.RNW, t.Exception, $"Exception in fire and forget asynchronous function");
                    },
                    TaskContinuationOptions.OnlyOnFaulted);
            }
        }

        /// <summary>
        /// Invokes a function on the main dispatcher and asynchronously returns the
        /// result.
        /// </summary>
        /// <typeparam name="T">Function return type.</typeparam>
        /// <param name="func">The function to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the main dispatcher.</param>
        /// <returns>A task to await the result.</returns>
        public static Task<T> CallOnDispatcher<T>(Func<T> func, bool allowInlining = false)
        {
            return CallOnDispatcher(MainDispatcher, func, allowInlining);
        }

        /// <summary>
        /// Invokes a function on a specified dispatcher and asynchronously returns the
        /// result.
        /// </summary>
        /// <typeparam name="T">Function return type.</typeparam>
        /// <param name="dispatcher">The CoreDispatcher to be used.</param>
        /// <param name="func">The function to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        /// <returns>A task to await the result.</returns>
        public static Task<T> CallOnDispatcher<T>(CoreDispatcher dispatcher, Func<T> func, bool allowInlining = false)
        {
            if (allowInlining && IsOnDispatcher(dispatcher))
            {
                try
                {
                    T result = func();
                    return Task.FromResult(result);
                }
                catch (Exception ex)
                {
                    return Task.FromException<T>(ex);
                }
            }
            else
            {
                var taskCompletionSource = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

                RunOnDispatcher(dispatcher, () =>
                {
                    try
                    {
                        var result = func();
                        taskCompletionSource.SetResult(result);
                    }
                    catch (Exception ex)
                    {
                        taskCompletionSource.SetException(ex);
                    }
                });

                return taskCompletionSource.Task;
            }
        }

        /// <summary>
        /// Cleans up the dispatcher helpers.
        /// </summary>
        public static void Reset()
        {
            s_isOnDispatcherThread.Dispose();
            s_isOnDispatcherThread = null;
            s_mainDispatcher = null;
        }
    }
}
