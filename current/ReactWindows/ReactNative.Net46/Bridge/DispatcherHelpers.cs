// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A set of helpers for dispatcher access.
    /// </summary>
    public static class DispatcherHelpers
    {
        private static Dispatcher s_mainDispatcher;

        /// <summary>
        /// Gets the main dispatcher for the app.
        /// </summary>
        public static Dispatcher MainDispatcher
        {
            get
            {
                AssertDispatcherSet();

                return s_mainDispatcher;
            }

            internal set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                if (value.Thread.GetApartmentState() != ApartmentState.STA)
                {
                    throw new ArgumentException("Dispatcher must be an STA thread");
                }

                s_mainDispatcher = value;
            }
        }

        /// <summary>
        /// Sets the main dispatcher.
        /// </summary>
        public static void Initialize()
        {
            s_mainDispatcher = Dispatcher.CurrentDispatcher;
        }

        /// <summary>
        /// Checks if the main dispatcher has been set
        /// </summary>
        /// <returns>
        /// <code>true</code> if the dispatcher has been set,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsDispatcherSet()
        {
            return s_mainDispatcher != null;
        }

        /// <summary>
        /// Asserts that the current thread has dispatcher access.
        /// </summary>
        public static void AssertOnDispatcher()
        {
            if (!IsOnDispatcher())
            {
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
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
            AssertDispatcherSet();

            return MainDispatcher.CheckAccess();
        }

        /// <summary>
        /// Checks if the current thread has access to the specified dispatcher.
        /// </summary>
        /// <param name="dispatcher">The dispatcher to be checked</param>
        /// <returns>
        /// <code>true</code> if the current thread has main dispatcher access,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsOnDispatcher(Dispatcher dispatcher)
        {
            return dispatcher.CheckAccess();
        }

        /// <summary>
        /// Asserts that the current thread has access to the dispatcher associated with the specified dependency object.
        /// </summary>
        /// <param name="dependencyObject">The dependency object who's dispatcher will be checked</param>
        public static void AssertOnDispatcher(DependencyObject dependencyObject)
        {
            AssertDispatcherSet();

            if (dependencyObject.Dispatcher != s_mainDispatcher)
            {
                throw new InvalidOperationException("Thread does not have dispatcher access (only main dispatcher is supported).");
            }
        }

        /// <summary>
        /// Invokes an action on the main dispatcher.
        /// </summary>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the main dispatcher.</param>
        public static void RunOnDispatcher(Action action, bool allowInlining = false)
        {
            AssertDispatcherSet();

            RunOnDispatcher(DispatcherPriority.Normal, action, allowInlining);
        }

        /// <summary>
        /// Invokes an action on the main dispatcher with custom priority.
        /// </summary>
        /// <param name="priority">The priority.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the main dispatcher.</param>
        public static void RunOnDispatcher(DispatcherPriority priority, Action action, bool allowInlining = false)
        {
            AssertDispatcherSet();

            if (allowInlining && IsOnDispatcher())
            {
                action();
            }
            else
            {
                MainDispatcher.InvokeAsync(action, priority).Task.ContinueWith(
                    t =>
                    {
                        Debug.Fail("Exception in fire and forget asynchronous function", t.Exception.ToString());
                    },
                    TaskContinuationOptions.OnlyOnFaulted);
            }

        }

        /// <summary>
        /// Invokes an action on a specified dispatcher.
        /// </summary>
        /// <param name="dispatcher">The dispatcher.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        public static void RunOnDispatcher(Dispatcher dispatcher, Action action, bool allowInlining = false)
        {
            RunOnDispatcher(dispatcher, DispatcherPriority.Normal, action, allowInlining);
        }

        /// <summary>
        /// Invokes an action on a specified dispatcher and priority
        /// </summary>
        /// <param name="dispatcher">The dispatcher.</param>
        /// <param name="priority">The priority.</param>
        /// <param name="action">The action to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        public static void RunOnDispatcher(Dispatcher dispatcher, DispatcherPriority priority, Action action, bool allowInlining = false)
        {
            AssertDispatcherSet();

            if (dispatcher != s_mainDispatcher)
            {
                throw new InvalidOperationException("Only main dispatcher is supported.");
            }

            RunOnDispatcher(priority, action, allowInlining);
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
        /// <param name="dispatcher">The Dispatcher to be used.</param>
        /// <param name="func">The function to invoke.</param>
        /// <param name="allowInlining">True if inlining is allowed when calling thread is on the same dispatcher as the one in the parameter.</param>
        /// <returns>A task to await the result.</returns>
        public static Task<T> CallOnDispatcher<T>(Dispatcher dispatcher, Func<T> func, bool allowInlining = false)
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

                RunOnDispatcher(dispatcher, DispatcherPriority.Normal, () =>
                {
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
                });

                return taskCompletionSource.Task;
            }
        }

        /// <summary>
        /// Cleans up the dispatcher helpers.
        /// </summary>
        /// <remarks>
        /// No-op on WPF
        /// </remarks>
        public static void Reset()
        {
            // No-op on WPF
        }

        private static void AssertDispatcherSet()
        {
            if (s_mainDispatcher == null)
            {
                throw new InvalidOperationException("Dispatcher has not been set");
            }
        }
    }
}
