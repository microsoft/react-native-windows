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
    public static class DispatcherHelpers
    {
        private static Dispatcher s_mainDispatcher;

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

        public static bool IsDispatcherSet()
        {
            return s_mainDispatcher != null;
        }

        public static void AssertOnDispatcher()
        {
            if (!IsOnDispatcher())
            {
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
        }

        public static bool IsOnDispatcher()
        {
            AssertDispatcherSet();

            return MainDispatcher.CheckAccess();
        }

        public static bool IsOnDispatcher(Dispatcher dispatcher)
        {
            return dispatcher.CheckAccess();
        }

        public static void AssertOnDispatcher(DependencyObject dependencyObject)
        {
            AssertDispatcherSet();

            if (dependencyObject.Dispatcher != s_mainDispatcher)
            {
                throw new InvalidOperationException("Thread does not have dispatcher access (only main dispatcher is supported).");
            }
        }

        public static void RunOnDispatcher(Action action, bool allowInlining = false)
        {
            AssertDispatcherSet();

            RunOnDispatcher(DispatcherPriority.Normal, action, allowInlining);
        }

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
        public static void RunOnDispatcher(Dispatcher dispatcher, Action action, bool allowInlining = false)
        {
            RunOnDispatcher(dispatcher, DispatcherPriority.Normal, action, allowInlining);
        }

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
                return Task.FromResult(func());
            }
            else
            {
                var taskCompletionSource = new TaskCompletionSource<T>();

                RunOnDispatcher(dispatcher, DispatcherPriority.Normal, () =>
                {
                    var result = func();

                    // TaskCompletionSource<T>.SetResult can call continuations
                    // on the awaiter of the task completion source.
                    Task.Run(() => taskCompletionSource.SetResult(result));
                });

                return taskCompletionSource.Task;
            }
        }

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
