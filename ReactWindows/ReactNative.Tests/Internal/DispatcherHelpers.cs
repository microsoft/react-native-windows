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
            var tcs = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(() =>
            {
                try
                {
                    var result = func();
                    tcs.SetResult(result);
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

        public static async Task CallOnDispatcherAsync(Func<Task> asyncFunc)
        {
            var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(async () =>
            {
                try
                {
                    await asyncFunc();
                    tcs.SetResult(true);
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            }).ConfigureAwait(false);

            await tcs.Task.ConfigureAwait(false);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(Func<Task<T>> asyncFunc)
        {
            var tcs = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(async () =>
            {
                try
                {
                    var result = await asyncFunc();
                    tcs.SetResult(result);
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

    }
}
