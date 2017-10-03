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

            var actionQueue = new ActionQueue(ex => { });
            AssertEx.Throws<ArgumentNullException>(
                () => actionQueue.Dispatch(null),
                ex => Assert.AreEqual("action", ex.ParamName));
        }

        [TestMethod]
        public void ActionQueue_CreateUiThread_ThrowsNotSupported()
        {
            AssertEx.Throws<NotSupportedException>(() => ActionQueueSpec.Create("ui", ActionQueueKind.Dispatcher));
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

            var queueThreads = new[]
            {
                uiThread,
                layoutThread,
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

            var queueThreads = new[]
            {
                uiThread,
                layoutThread,
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

        [TestMethod]
        public async Task ActionQueue_OneAtATime()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var enter = new AutoResetEvent(false);
            var exit = new AutoResetEvent(false);

            var queueThreads = new[] 
            {
                uiThread,
                layoutThread,
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

        [TestMethod]
        public async Task ActionQueue_Dispose()
        {
            var onError = new Action<Exception>(ex => Assert.Fail());
            var uiThread = await CallOnDispatcherAsync(() => new DispatcherActionQueue(onError));
            var layoutThread = await CallOnDispatcherAsync(() => new LayoutActionQueue(onError));
            var backgroundThread = new ActionQueue(onError, NewThreadScheduler.Default);
            var taskPoolThread = new ActionQueue(onError);

            var queueThreads = new[]
            {
                uiThread,
                layoutThread,
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
            }
        }
    }
}
