// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

            await RunOnDispatcherAsync(async () =>
            {
                try
                {
                    var result = func();
                    await Task.Run(() => tcs.SetResult(result));
                }
                catch (Exception ex)
                {
                    await Task.Run(() => tcs.SetException(ex));
                }
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

        public static async Task CallOnDispatcherAsync(Func<Task> asyncFunc)
        {
            var tcs = new TaskCompletionSource<bool>();

            await RunOnDispatcherAsync(async () =>
            {
                try
                {
                    await asyncFunc();
                    await Task.Run(() => tcs.SetResult(true));
                }
                catch (Exception ex)
                {
                    await Task.Run(() => tcs.SetException(ex));
                }
            }).ConfigureAwait(false);

            await tcs.Task.ConfigureAwait(false);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(Func<Task<T>> asyncFunc)
        {
            var tcs = new TaskCompletionSource<T>();

            await RunOnDispatcherAsync(async () =>
            {
                try
                {
                    var result = await asyncFunc();
                    await Task.Run(() => tcs.SetResult(result));
                }
                catch (Exception ex)
                {
                    await Task.Run(() => tcs.SetException(ex));
                }
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

    }
}
