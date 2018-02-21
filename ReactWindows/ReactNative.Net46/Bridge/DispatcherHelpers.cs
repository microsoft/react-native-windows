using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

// NOTE: Made DispatcherHelpers public for use in CursorModule. Once we upgrade to 0.49 all of this changes anyways.

namespace ReactNative.Bridge
{
    /// <summary>
    /// Utility for managing the application dispatcher
    /// </summary>
    public static class DispatcherHelpers
    {
        private static Dispatcher _dispatcher;

        /// <summary>
        /// Gets the current dispatcher
        /// </summary>
        internal static Dispatcher CurrentDispatcher
        {
            get
            {
                AssertDispatcherSet();

                return _dispatcher;
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                if (value.Thread.GetApartmentState() != ApartmentState.STA)
                {
                    throw new ArgumentException("Dispatcher must be an STA thread");
                }

                _dispatcher = value;
            }
        }

        /// <summary>
        /// Gets a value indicating whetehr the dispatcher has been set
        /// </summary>
        /// <returns></returns>
        public static bool IsDispatcherSet()
        {
            return _dispatcher != null;
        }

        /// <summary>
        /// Assert that the current thread is running on the dispatcher
        /// </summary>
        public static void AssertOnDispatcher()
        {
            if (!IsOnDispatcher())
            {
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
        }

        /// <summary>
        /// Gets a value indicating whether the current thread is the dispatcher thread
        /// </summary>
        /// <returns></returns>
        public static bool IsOnDispatcher()
        {
            AssertDispatcherSet();

            return CurrentDispatcher.CheckAccess();
        }

        /// <summary>
        /// Invoke the action on the dispatcher thread
        /// </summary>
        /// <param name="action">The action to invoke</param>
        public static async void RunOnDispatcher(Action action)
        {
            AssertDispatcherSet();

            await CurrentDispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
        }

        /// <summary>
        /// Invoke the function on the dispatcher
        /// </summary>
        /// <typeparam name="T">The return type of the result</typeparam>
        /// <param name="func">The function to invoke</param>
        /// <returns></returns>
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

        private static void AssertDispatcherSet()
        {
            if (_dispatcher == null)
            {
                throw new InvalidOperationException("Dispatcher has not been set");
            }
        }
    }
}
