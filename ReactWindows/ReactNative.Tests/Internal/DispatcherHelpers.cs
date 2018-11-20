// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace ReactNative.Tests
{
    static class DispatcherHelpers
    {
        public static Task RunOnDispatcherAsync(Action action)
        {
            return RunOnDispatcherAsync(App.Dispatcher, action);
        }

        public static async Task RunOnDispatcherAsync(CoreDispatcher dispatcher, Action action)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(action)).AsTask().ConfigureAwait(false);
        }

        public static Task<T> CallOnDispatcherAsync<T>(Func<T> func)
        {
            return CallOnDispatcherAsync<T>(App.Dispatcher, func);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(CoreDispatcher dispatcher, Func<T> func)
        {
            var tcs = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(dispatcher, () =>
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

        public static Task CallOnDispatcherAsync(Func<Task> asyncFunc)
        {
            return CallOnDispatcherAsync(App.Dispatcher, asyncFunc);
        }

        public static async Task CallOnDispatcherAsync(CoreDispatcher dispatcher, Func<Task> asyncFunc)
        {
            var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(dispatcher, async () =>
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

        public static Task<T> CallOnDispatcherAsync<T>(Func<Task<T>> asyncFunc)
        {
            return CallOnDispatcherAsync(App.Dispatcher, asyncFunc);
        }

        public static async Task<T> CallOnDispatcherAsync<T>(CoreDispatcher dispatcher, Func<Task<T>> asyncFunc)
        {
            var tcs = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(dispatcher, async () =>
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
