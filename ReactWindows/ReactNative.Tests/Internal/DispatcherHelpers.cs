using System;
using System.Threading.Tasks;
using Windows.System.Threading;
using Windows.UI.Core;

namespace ReactNative.Tests
{
    static class DispatcherHelpers
    {
        public static async Task RunOnDispatcherAsync(Action action)
        {
            await App.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(action));
        }

        public static async Task<T> CallOnDispatcherAsync<T>(Func<T> func)
        {
            var tcs = new TaskCompletionSource<T>();

            await RunOnDispatcherAsync(() =>
            {
                var result = func();
#pragma warning disable CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
                ThreadPool.RunAsync(_ => tcs.SetResult(result));
#pragma warning restore CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
            });

            return await tcs.Task;
        }
    }
}
