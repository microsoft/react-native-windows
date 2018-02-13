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
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
#endif
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
                if (MainDispatcher.HasThreadAccess)
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
            await MainDispatcher.RunAsync(priority, action).AsTask().ConfigureAwait(false);
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
            s_mainDispatcher = null;
        }
    }
}
