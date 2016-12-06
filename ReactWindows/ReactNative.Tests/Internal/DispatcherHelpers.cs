using System;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace ReactNative.Tests
{
    static class DispatcherHelpers
    {
        public static async Task RunOnDispatcherAsync(Action action)
        {
            await App.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(action)).AsTask().ConfigureAwait(false);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(Func<T> func)
        {
            var tcs = new TaskCompletionSource<T>();

            await RunOnDispatcherAsync(() =>
            {
                var result = func();

                Task.Run(() => tcs.SetResult(result));
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Reviewed.")]
        public static async Task CallOnDispatcherAsync(Func<Task> asyncFunc)
        {
            var tcs = new TaskCompletionSource<bool>();

#pragma warning disable AvoidAsyncVoid
            await RunOnDispatcherAsync(async () =>
            {
                await asyncFunc();
                await Task.Run(() => tcs.SetResult(true));
            }).ConfigureAwait(false);
#pragma warning restore AvoidAsyncVoid

            await tcs.Task.ConfigureAwait(false);
        }
    }
}
