// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NMock;
using NUnit.Framework;
using ReactNative.Bridge.Queue;
using System;
using System.Reactive.Concurrency;
using System.Reactive.Disposables;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using static ReactNative.Tests.DispatcherHelpers;

namespace ReactNative.Tests.Bridge.Queue
{
    [TestFixture]
    public class ActionQueueTests
    {
        [Test]
        public void ActionQueue_ArgumentChecks()
        {
            ArgumentNullException ex = Assert.Throws<ArgumentNullException>(
                () => new ActionQueue(null, Scheduler.Default));

            Assert.AreEqual("onError", ex.ParamName);

            ArgumentNullException ex3 = Assert.Throws<ArgumentNullException>(
                () => new ActionQueue(_ => { }, null));

            Assert.AreEqual("scheduler", ex3.ParamName);
        }

        // ToDo: Conflicts with UIManagerModule tests [SetUp]
        public void SetUp()
        {
            var waitForApplicationRun = new ManualResetEventSlim();
            Task.Run(() =>
            {
                var app = new Application();
                app.Startup += (s, e) => { waitForApplicationRun.Set(); };
                app.Run();
            });
        }

        [Ignore("Flaky"), Test]
        public async Task ActionQueue_IsOnThread()
        {
            var thrown = 0;
            var onError = new Action<Exception>(ex => thrown++);
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var queueThreads = new[]
            {
                uiThread,
                backgroundThread,
                taskPoolThread
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

        [Ignore("Flaky"), Test]
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
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var queueThreads = new[]
            {
                uiThread,
                backgroundThread,
                taskPoolThread
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

        [Ignore("Flaky"), Test]
        public async Task ActionQueue_OneAtATime()
        {
            var onError = new Action<Exception>(ex => { });
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var enter = new AutoResetEvent(false);
            var exit = new AutoResetEvent(false);

            var queueThreads = new[] 
            {
                uiThread,
                backgroundThread,
                taskPoolThread
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

        [Ignore("Flaky"), Test]
        public async Task ActionQueue_Dispose()
        {
            var onError = new Action<Exception>(ex => { });
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var queueThreads = new[]
            {
                uiThread,
                backgroundThread,
                taskPoolThread
            };

            using (new CompositeDisposable(queueThreads))
            {
                var waitHandle = new AutoResetEvent(false);

                foreach (var queueThread in queueThreads)
                {
                    queueThread.Dispose();
                    queueThread.Dispatch(() =>
                    {
                        waitHandle.Set();
                    });

                    Assert.IsFalse(waitHandle.WaitOne(500));
                }
                Assert.True(true);
            }
        }

        //[TearDown]
        public void TearDown()
        {
            var waitForApplicationExit = new ManualResetEventSlim();
            Task.Run(() =>
            {
                Application.Current.Exit += (s, e) => { waitForApplicationExit.Set(); };
                Application.Current.Shutdown();
            });
        }
    }
}
