// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Bridge.Queue;
using System;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Threading;
using System.Threading.Tasks;
using static ReactNative.Tests.DispatcherHelpers;

namespace ReactNative.Tests.Bridge.Queue
{
    [TestClass]
    public class ActionQueueTests
    {
        [TestMethod]
        public void ActionQueue_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => new ActionQueue(null),
                ex => Assert.AreEqual("onError", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => new ActionQueue(null, Scheduler.Default),
                ex => Assert.AreEqual("onError", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => new ActionQueue(ex => { }, null),
                ex => Assert.AreEqual("scheduler", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => new LimitedConcurrencyActionQueue(null),
                ex => Assert.AreEqual("onError", ex.ParamName));

            var actionQueue = new ActionQueue(ex => { });
            AssertEx.Throws<ArgumentNullException>(
                () => actionQueue.Dispatch(null),
                ex => Assert.AreEqual("action", ex.ParamName));
        }

        [TestMethod]
        public async Task ActionQueue_IsOnThread()
        {
            var thrown = 0;
            var onError = new Action<Exception>(ex => thrown++);
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                var countdown = new CountdownEvent(queueThreads.Length);
                foreach (var queueThread in queueThreads)
                {
                    queueThread.Dispatch(() =>
                    {
                        Assert.IsTrue(queueThread.IsOnThread());
                        countdown.Signal();
                    });
                }

                Assert.IsTrue(countdown.Wait(5000));
                Assert.AreEqual(0, thrown);
            }
        }

        [TestMethod]
        public async Task ActionQueue_HandlesException()
        {
            var exception = new Exception();
            var countdown = new CountdownEvent(1);
            var onError = new Action<Exception>(ex =>
            {
                Assert.AreSame(exception, ex);
                countdown.Signal();
            });

            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                countdown.AddCount(queueThreads.Length - 1);

                foreach (var queueThread in queueThreads)
                {
                    queueThread.Dispatch(() => { throw exception; });
                }

                Assert.IsTrue(countdown.Wait(5000));
            }
        }

        [TestMethod]
        public async Task ActionQueue_OneAtATime()
        {
            var enter = new AutoResetEvent(false);
            var exit = new AutoResetEvent(false);

            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                foreach (var queueThread in queueThreads)
                {
                    var count = 10;
                    for (var i = 0; i < count; ++i)
                    {
                        queueThread.Dispatch(() => { enter.Set(); exit.WaitOne(); });
                    }

                    for (var i = 0; i < count; ++i)
                    {
                        Assert.IsTrue(enter.WaitOne());
                        Assert.IsFalse(enter.WaitOne(100));
                        exit.Set();
                    }
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_DispatchToSelf()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                foreach (var queue in queueThreads)
                {
                    var waitHandle = new AutoResetEvent(false);
                    queue.Dispatch(() =>
                        queue.Dispatch(() =>
                            waitHandle.Set()));
                    Assert.IsTrue(waitHandle.WaitOne());
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_RunAsync_Throws()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                foreach (var queue in queueThreads)
                {
                    var exception = new InvalidOperationException();
                    await AssertEx.ThrowsAsync<InvalidOperationException>(
                        () => queue.RunAsync(() => throw exception),
                        ex => Assert.AreSame(exception, ex));
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_Dispose()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                var waitHandle = new AutoResetEvent(false);
                foreach (var queueThread in queueThreads)
                {
                    queueThread.Dispose();
                    queueThread.Dispatch(() => waitHandle.Set());
                    Assert.IsFalse(waitHandle.WaitOne(100));
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_Dispose_Idempotent()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                var waitHandle = new AutoResetEvent(false);
                foreach (var queueThread in queueThreads)
                {
                    queueThread.Dispose();
                    queueThread.Dispose();
                    queueThread.Dispatch(() => waitHandle.Set());
                    Assert.IsFalse(waitHandle.WaitOne(100));
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_DisposeSelf()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                foreach (var queue in queueThreads)
                {
                    var disposeTask = queue.RunAsync(() =>
                    {
                        queue.Dispose();
                        return true;
                    });

                    var task = await Task.WhenAny(disposeTask, Task.Delay(5000));
                    Assert.AreSame(disposeTask, task);

                    var wontRunTask = queue.RunAsync(() =>
                    {
                        return true;
                    });

                    task = await Task.WhenAny(wontRunTask, Task.Delay(500));
                    Assert.AreNotSame(wontRunTask, task);
                }
            }
        }

        [TestMethod]
        public async Task ActionQueue_Dispose_WhileRunning()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);
            var limitedConcurrencyThread = new LimitedConcurrencyActionQueue(onError);

            var queueThreads = new IActionQueue[]
            {
                uiThread,
                layoutThread,
                backgroundThread,
                taskPoolThread,
                limitedConcurrencyThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                foreach (var queue in queueThreads)
                {
                    var enter = new AutoResetEvent(false);
                    var wait = new AutoResetEvent(false);
                    queue.Dispatch(() =>
                    {
                        enter.Set();
                        wait.WaitOne();
                    });

                    enter.WaitOne();
                    var task = Task.Run(() => queue.Dispose());
                    var anyTask = Task.WhenAny(task, Task.Delay(100));
                    Assert.AreNotSame(task, anyTask);
                    Assert.IsFalse(task.IsCompleted);
                    wait.Set();
                    await task;
                }
            }
        }
    }
}
