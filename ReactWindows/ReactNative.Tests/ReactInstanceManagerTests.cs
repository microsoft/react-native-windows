using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Tests
{
    [TestClass]
    public class ReactInstanceManagerTests
    {
        [TestMethod]
        public void ReactInstanceManager_Builder_SetterChecks()
        {
            AssertEx.Throws<InvalidOperationException>(
                () => new ReactInstanceManagerBuilder
                    {
                        JavaScriptBundleFile = "ms-appx:///Resources/main.jsbundle",
                    }.Build());

            AssertEx.Throws<InvalidOperationException>(
                () => new ReactInstanceManagerBuilder
                    {
                        InitialLifecycleState = LifecycleState.BeforeCreate,
                    }.Build());
        }

        [TestMethod]
        public async Task ReactInstanceManager_ArgumentChecks()
        {
            var manager = CreateReactInstanceManager();

            AssertEx.Throws<ArgumentNullException>(
                () => manager.AttachMeasuredRootView(null),
                ex => Assert.AreEqual("rootView", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => manager.CreateAllViewManagers(null),
                ex => Assert.AreEqual("reactContext", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => manager.DetachRootView(null),
                ex => Assert.AreEqual("rootView", ex.ParamName));

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateReactContextAsync()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateReactContextAsync_EnsuresOneCall()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var caught = false;
            await DispatcherHelpers.RunOnDispatcherAsync(async () =>
            {
                var task = manager.CreateReactContextAsync(CancellationToken.None);

                try
                {
                    await manager.CreateReactContextAsync(CancellationToken.None);
                }
                catch (InvalidOperationException)
                {
                    caught = true;
                }
            });

            Assert.IsTrue(caught);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_RecreateReactContextAsync()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var task = default(Task<ReactContext>);
            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                task = manager.CreateReactContextAsync(CancellationToken.None);
                return await manager.RecreateReactContextAsync(CancellationToken.None);
            });

            var initialReactContext = await task;
            Assert.IsNotNull(reactContext);
            Assert.AreEqual(jsBundleFile, manager.SourceUrl);
            Assert.AreNotEqual(initialReactContext, reactContext);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_RecreateReactContextAsync_EnsuresCalledOnce()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var caught = false;
            await DispatcherHelpers.RunOnDispatcherAsync(async () =>
            {
                try
                {
                    await manager.RecreateReactContextAsync(CancellationToken.None);
                }
                catch (InvalidOperationException)
                {
                    caught = true;
                }
            });

            Assert.IsTrue(caught);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_OnBackPressed_NoContext()
        {
            var waitHandle = new AutoResetEvent(false);
            var manager = CreateReactInstanceManager();
            await DispatcherHelpers.RunOnDispatcherAsync(() =>
            {
                manager.OnResume(() => waitHandle.Set());
                manager.OnBackPressed();
            });

            Assert.IsTrue(waitHandle.WaitOne());

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_OnDestroy_CreateReactContextAsync()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            Assert.IsNotNull(reactContext);
            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);

            reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            Assert.IsNotNull(reactContext);
            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_DisposeAsync_WhileBusy()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            var e = new AutoResetEvent(false);
            reactContext.RunOnNativeModulesQueueThread(() =>
            {
                e.WaitOne();
                var x = DispatcherHelpers.CallOnDispatcherAsync(() =>
                {
                    return 42;
                }).Result;
            });

            var task = DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                e.Set();
                await manager.DisposeAsync();
            });

            var completedTask = await Task.WhenAny(
                Task.Delay(5000),
                task);

            Assert.IsTrue(task.IsCompleted);
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateReactContextAsync_Canceled()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var cancellationTokenSource = new CancellationTokenSource();
            cancellationTokenSource.Cancel();
            await AssertEx.ThrowsAsync<OperationCanceledException>(
                async () => await DispatcherHelpers.CallOnDispatcherAsync(() =>
                    manager.CreateReactContextAsync(cancellationTokenSource.Token)),
                ex => Assert.AreEqual(cancellationTokenSource.Token, ex.CancellationToken));

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Unfinished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var initialContextTask = DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.GetReactContextAsync(CancellationToken.None));
            var initialContext = await initialContextTask;

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }


        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Finished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var initialContext = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.GetReactContextAsync(CancellationToken.None));

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Fail()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var caught = false;
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                try
                {
                    await manager.GetReactContextAsync(CancellationToken.None);
                }
                catch (InvalidOperationException)
                {
                    caught = true;
                }
            });

            Assert.IsTrue(caught);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetOrCreateReactContextAsync_Create()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(() => manager.GetOrCreateReactContextAsync(CancellationToken.None));
            Assert.IsNotNull(reactContext);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetOrCreateReactContextAsync_Get()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var initialContext = default(ReactContext);
            var context = default(ReactContext);
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var initialContextTask = manager.CreateReactContextAsync(CancellationToken.None);
                context = await manager.GetOrCreateReactContextAsync(CancellationToken.None);
                initialContext = await initialContextTask;
            });

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        private static ReactInstanceManager CreateReactInstanceManager()
        {
            return CreateReactInstanceManager("ms-appx:///Resources/main.jsbundle");
        }

        private static ReactInstanceManager CreateReactInstanceManager(string jsBundleFile)
        {
            return new ReactInstanceManagerBuilder
            {
                InitialLifecycleState = LifecycleState.Resumed,
                JavaScriptBundleFile = jsBundleFile,
            }.Build();
        }
    }
}
