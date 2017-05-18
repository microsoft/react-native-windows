using System;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

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
            return CoreWindow.GetForCurrentThread()?.Dispatcher != null;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Fire-and-forget method.")]
#pragma warning disable AvoidAsyncVoid
        public static async void RunOnDispatcher(DispatchedHandler action)
#pragma warning restore AvoidAsyncVoid
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action).AsTask().ConfigureAwait(false);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(Func<T> func)
        {
            var taskCompletionSource = new TaskCompletionSource<T>();

            RunOnDispatcher(() =>
            {
                var result = func();

                // TaskCompletionSource<T>.SetResult can call continuations
                // on the awaiter of the task completion source.
                Task.Run(() => taskCompletionSource.SetResult(result));
            });

            return await taskCompletionSource.Task;
        }
    }
}
