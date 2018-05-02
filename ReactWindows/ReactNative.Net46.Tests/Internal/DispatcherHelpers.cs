// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace ReactNative.Tests
{
    static class DispatcherHelpers
    {
        public static async Task RunOnDispatcherAsync(Action action)
        {
            Dispatcher dispatcher = Application.Current != null
                ? Application.Current.Dispatcher
                : Dispatcher.CurrentDispatcher;
            if (dispatcher.CheckAccess())
            {
                dispatcher.Invoke(action);
            }
            else
            {
                await dispatcher.InvokeAsync(action).Task.ConfigureAwait(false);
            }
        }

        // Single thread apartment attribute for sake of being able to access UI elements on the same thread they're created from
        [Apartment(ApartmentState.STA)]
        public static async Task<T> CallOnDispatcherAsync<T>(Func<T> func)
        {
            var tcs = new TaskCompletionSource<T>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(() =>
            {
                var result = func();
                tcs.SetResult(result);
            }).ConfigureAwait(false);

            return await tcs.Task.ConfigureAwait(false);
        }

        public static async Task CallOnDispatcherAsync(Func<Task> asyncFunc)
        {
            var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

            await RunOnDispatcherAsync(async () =>
            {
                await asyncFunc();
                tcs.SetResult(true);
            }).ConfigureAwait(false);

            await tcs.Task.ConfigureAwait(false);
        }
    }
}
