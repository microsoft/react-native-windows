// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.Modules.Network;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Tests.Modules.Network
{
    [TestFixture]
    public class TaskCancellationManagerTests
    {
        [Test]
        public void TaskCancellationManager_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => new TaskCancellationManager<int>(null),
                ex => Assert.AreEqual("keyComparer", ex.ParamName));
        }

        [Test]
        public async Task TaskCancellationManager_CancelledAfterCompleted()
        {
            var mgr = new TaskCancellationManager<int>();
            await mgr.AddAndInvokeAsync(42, _ => Task.CompletedTask);
            mgr.Cancel(42);

            // Not throwing implies success
        }

        [Test]
        public void TaskCancellationManager_CancelInvalidId()
        {
            var mgr = new TaskCancellationManager<int>();
            mgr.Cancel(42);

            // Not throwing implies success 
        }

        [Test]
        public async Task TaskCancellationManager_CancelTask()
        {
            using (var enter = new AutoResetEvent(false))
            using (var exit = new AutoResetEvent(false))
            {
                var mgr = new TaskCancellationManager<int>();
                var t = mgr.AddAndInvokeAsync(42, async token =>
                {
                    var tcs = new TaskCompletionSource<bool>();
                    using (token.Register(() => tcs.SetResult(true)))
                    {
                        enter.Set();
                        await tcs.Task;
                        exit.Set();
                    }
                });

                Assert.IsTrue(enter.WaitOne());
                mgr.Cancel(42);
                Assert.IsTrue(exit.WaitOne());
                await t;
            }
        }

        [Test]
        public async Task TaskCancellationManager_CleanedUpAfterComplete()
        {
            using (var enter = new AutoResetEvent(false))
            {
                var mgr = new TaskCancellationManager<int>();
                var t = mgr.AddAndInvokeAsync(42, token =>
                {
                    return Task.Run(() =>
                    {
                        enter.WaitOne();
                    });
                });

                Assert.IsNotNull(t);
                Assert.AreEqual(1, mgr.PendingOperationCount);
                enter.Set();
                await t;
                Assert.AreEqual(0, mgr.PendingOperationCount);
            }
        }

        [Test]
        public async Task TaskCancellationManager_CleanedUpAfterError()
        {
            using (var enter = new AutoResetEvent(false))
            {
                var mgr = new TaskCancellationManager<int>();
                var t = mgr.AddAndInvokeAsync(42, token =>
                {
                    return Task.Run(() =>
                    {
                        enter.WaitOne();
                        throw new InvalidOperationException();
                    });
                });

                Assert.IsNotNull(t);
                Assert.AreEqual(1, mgr.PendingOperationCount);
                enter.Set();
                await AssertEx.ThrowsAsync<InvalidOperationException>(async () => await t);
                Assert.AreEqual(0, mgr.PendingOperationCount);
            }
        }

        [Test]
        public async Task TaskCancellationManager_CancelAllTasks()
        {
            using (var enter = new AutoResetEvent(false))
            using (var exit = new AutoResetEvent(false))
            {
                var mgr = new TaskCancellationManager<int>();
                var t = mgr.AddAndInvokeAsync(42, async token =>
                {
                    var tcs = new TaskCompletionSource<bool>();
                    using (token.Register(() => tcs.SetResult(true)))
                    {
                        enter.Set();
                        await tcs.Task;
                        exit.Set();
                    }
                });

                Assert.IsTrue(enter.WaitOne());
                mgr.CancelAllTasks();
                Assert.IsTrue(exit.WaitOne());
                await t;
            }
        }

        [Test]
        public async Task TaskCancellationManager_CancelAfterCancelAllTasks()
        {
            using (var enter = new AutoResetEvent(false))
            using (var exit = new AutoResetEvent(false))
            {
                var mgr = new TaskCancellationManager<int>();
                var t = mgr.AddAndInvokeAsync(42, async token =>
                {
                    var tcs = new TaskCompletionSource<bool>();
                    using (token.Register(() => tcs.SetResult(true)))
                    {
                        enter.Set();
                        await tcs.Task;
                        exit.Set();
                    }
                });

                Assert.IsTrue(enter.WaitOne());
                mgr.CancelAllTasks();
                mgr.Cancel(42);
                Assert.IsTrue(exit.WaitOne());
                await t;
            }
        }

        [Test]
        public async Task TaskCancellationManager_CancelAllTasks_Many()
        {
            var count = 100;
            using (var enter = new CountdownEvent(count))
            using (var exit = new CountdownEvent(count))
            {
                var mgr = new TaskCancellationManager<int>();

                var tasks = new List<Task>(count);
                for (var i = 0; i < count; ++i)
                {
                    tasks.Add(mgr.AddAndInvokeAsync(i, async token =>
                    {
                        var tcs = new TaskCompletionSource<bool>();
                        using (token.Register(() => tcs.SetResult(true)))
                        {
                            enter.Signal();
                            await tcs.Task;
                            exit.Signal();
                        }
                    }));

                }

                Assert.IsTrue(enter.WaitHandle.WaitOne());      
                Assert.AreEqual(count, mgr.PendingOperationCount);
                mgr.CancelAllTasks();
                Assert.IsTrue(exit.WaitHandle.WaitOne());
                await Task.WhenAll(tasks);
                Assert.AreEqual(0, mgr.PendingOperationCount);
            }
        }
    }
}
