using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace ReactNative.Bridge
{
    static class DispatcherHelpers
    {
        public static void AssertOnDispatcher()
        {
            if (!IsOnDispatcher())
            {
                throw new InvalidOperationException("Thread does not have dispatcher access.");
            }
        }

        public static bool IsOnDispatcher()
        {
            return Application.Current.Dispatcher.CheckAccess();
        }

        public static async void RunOnDispatcher(Action action, bool runAsync = true)
        {
            if (action == null)
            {
                throw new InvalidOperationException("No Action");
            }

            if (IsOnDispatcher())
            {
                if (runAsync)
                {
                    await Application.Current.Dispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
                }
                else
                {
                    action.Invoke();
                }
            }
            else
            {
                if (runAsync)
                {
                    await Application.Current.Dispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
                }
                else
                {
                    Application.Current.Dispatcher.Invoke(action);
                }
            }
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
    }
}
