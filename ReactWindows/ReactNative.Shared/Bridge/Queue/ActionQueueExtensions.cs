using System;
using System.Threading.Tasks;

namespace ReactNative.Bridge.Queue
{
    /// <summary>
    /// Extension methods for <see cref="IActionQueue"/>s.
    /// </summary>
    public static class ActionQueueExtensions
    {
        /// <summary>
        /// Asserts <see cref="IActionQueue.IsOnThread"/>, throwing if the <b>false</b>.
        /// </summary>
        /// <param name="actionQueue">The message queue thread.</param>
        /// <exception cref="InvalidOperationException">
        /// Thrown if the assertion fails.
        /// </exception>
        public static void AssertOnThread(this IActionQueue actionQueue)
        {
            if (!actionQueue.IsOnThread())
            {
                throw new InvalidOperationException("Thread access assertion failed.");
            }
        }

        /// <summary>
        /// Calls a function on a message queue and returns a task to await the response.
        /// </summary>
        /// <typeparam name="T">Type of response.</typeparam>
        /// <param name="actionQueue">The message queue thread.</param>
        /// <param name="func">The function.</param>
        /// <returns>A task to await the result.</returns>
        public static Task<T> RunAsync<T>(this IActionQueue actionQueue, Func<T> func)
        {
            var taskCompletionSource = new TaskCompletionSource<T>();

            actionQueue.Dispatch(() =>
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
