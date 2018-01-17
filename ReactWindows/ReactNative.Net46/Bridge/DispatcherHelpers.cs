using System;
using System.Threading;
using System.Threading.Tasks;
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

        public static async void RunOnDispatcher(Action action)
        {
            AssertDispatcherSet();

            await MainDispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
        }

        public static async void RunOnDispatcher(DispatcherPriority priority, Action action)
        {
            AssertDispatcherSet();

            await MainDispatcher.InvokeAsync(action, priority).Task.ConfigureAwait(false);
        }

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
