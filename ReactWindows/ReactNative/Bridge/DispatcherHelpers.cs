using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace ReactNative.Bridge
{
    /// <summary>
    /// A set of helpers for dispatcher access.
    /// </summary>
    public static class DispatcherHelpers
    {
        private static ThreadLocal<bool> s_isOnDispatcherThread;

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
        /// <code>true</code> if the current thread has dispatcher access,
        /// otherwise <code>false</code>.
        /// </returns>
        public static bool IsOnDispatcher()
        {
            if (s_isOnDispatcherThread == null)
            {
                if (CoreWindow.GetForCurrentThread()?.Dispatcher == CoreApplication.MainView.CoreWindow.Dispatcher)
                {
                    s_isOnDispatcherThread = new ThreadLocal<bool>();
                    s_isOnDispatcherThread.Value = true;
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
        /// Invokes an action on the dispatcher.
        /// </summary>
        /// <param name="action">The action to invoke.</param>
        public static void RunOnDispatcher(DispatchedHandler action)
        {
            RunOnDispatcher(CoreDispatcherPriority.Normal, action);
        }

        /// <summary>
        /// Invokes an action on the dispatcher.
        /// </summary>
        /// <param name="priority">The priority.</param>
        /// <param name="action">The action to invoke.</param>
        public static async void RunOnDispatcher(CoreDispatcherPriority priority, DispatchedHandler action)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(priority, action).AsTask().ConfigureAwait(false);
        }

        /// <summary>
        /// Invokes a function on the dispatcher and asynchronously returns the
        /// result.
        /// </summary>
        /// <typeparam name="T">Function return type.</typeparam>
        /// <param name="func">The function to invoke.</param>
        /// <returns>A task to await the result.</returns>
        public static Task<T> CallOnDispatcher<T>(Func<T> func)
        {
            var taskCompletionSource = new TaskCompletionSource<T>();

            RunOnDispatcher(() =>
            {
                var result = func();

                // TaskCompletionSource<T>.SetResult can call continuations
                // on the awaiter of the task completion source.
                Task.Run(() => taskCompletionSource.SetResult(result));
            });

            return taskCompletionSource.Task;
        }

        /// <summary>
        /// Cleans up the dispatcher helpers.
        /// </summary>
        public static void Reset()
        {
            s_isOnDispatcherThread.Dispose();
            s_isOnDispatcherThread = null;
        }
    }
}
