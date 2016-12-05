using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace ReactNative.Bridge
{
    static class DispatcherHelpers
    {
        private static Dispatcher _dispatcher;

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

        public static bool IsDispatcherSet()
        {
            return _dispatcher != null;
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

            return CurrentDispatcher.CheckAccess();
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Reviewed.")]
#pragma warning disable AvoidAsyncVoid
        public static async void RunOnDispatcher(Action action)
#pragma warning restore AvoidAsyncVoid
        {
            AssertDispatcherSet();

            await CurrentDispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
        }

        public static Task<T> CallOnDispatcherAsync<T>(Func<T> func)
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
