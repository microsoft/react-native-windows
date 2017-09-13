using System;
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
            return CoreWindow.GetForCurrentThread()?.Dispatcher == CoreApplication.MainView.CoreWindow.Dispatcher;
        }

        /// <summary>
        /// Invokes an action on the dispatcher.
        /// </summary>
        /// <param name="action">The action to invoke.</param>
        public static async void RunOnDispatcher(DispatchedHandler action)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action).AsTask().ConfigureAwait(false);
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
    }
}
