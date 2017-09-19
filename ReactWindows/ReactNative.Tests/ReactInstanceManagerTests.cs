using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Common;
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
        public async Task ReactInstanceManager_CreateInBackground()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var waitHandle = new AutoResetEvent(false);
            manager.ReactContextInitialized += (sender, args) => waitHandle.Set();

            await DispatcherHelpers.RunOnDispatcherAsync(
                () => manager.CreateReactContextInBackground());

            Assert.IsTrue(waitHandle.WaitOne());
            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateInBackground_EnsuresOneCall()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var waitHandle = new AutoResetEvent(false);
            manager.ReactContextInitialized += (sender, args) => waitHandle.Set();

            var caught = false;
            await DispatcherHelpers.RunOnDispatcherAsync(async () =>
            {
                manager.CreateReactContextInBackground();

                try
                {
                    await manager.CreateReactContextInBackgroundAsync();
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
        public async Task ReactInstanceManager_RecreateInBackground()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var waitHandle = new AutoResetEvent(false);
            manager.ReactContextInitialized += (sender, args) => waitHandle.Set();

            await DispatcherHelpers.RunOnDispatcherAsync(() =>
            {
                manager.CreateReactContextInBackground();
                manager.RecreateReactContextInBackground();
            });

            Assert.IsTrue(waitHandle.WaitOne());
            Assert.IsTrue(waitHandle.WaitOne());
            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_RecreateInBackground_EnsuresCalledOnce()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var caught = false;
            await DispatcherHelpers.RunOnDispatcherAsync(async () =>
            {
                try
                {
                    await manager.RecreateReactContextInBackgroundAsync();
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
        public async Task ReactInstanceManager_OnDestroy_CreateInBackground()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var waitHandle = new AutoResetEvent(false);
            manager.ReactContextInitialized += (sender, args) => waitHandle.Set();

            await DispatcherHelpers.RunOnDispatcherAsync(
                () => manager.CreateReactContextInBackground());

            Assert.IsTrue(waitHandle.WaitOne());
            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);

            await DispatcherHelpers.RunOnDispatcherAsync(
                () => manager.CreateReactContextInBackground());

            Assert.IsTrue(waitHandle.WaitOne());

            await DispatcherHelpers.CallOnDispatcherAsync(manager.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstanceManager_DisposeAsync_WhileBusy()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            var manager = CreateReactInstanceManager(jsBundleFile);

            var initializedEvent = new AutoResetEvent(false);
            manager.ReactContextInitialized += (sender, args) => initializedEvent.Set();

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                await manager.CreateReactContextInBackgroundAsync();
            });

            var e = new AutoResetEvent(false);

            initializedEvent.WaitOne();
            manager.CurrentReactContext.RunOnNativeModulesQueueThread(() =>
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
