// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
        private class LifecycleEventsListener :
            ILifecycleEventListener,
            IBackgroundEventListener,
            IDisposable
        {
            public enum Step
            {
                Resume,
                Suspend,
                EnteredBackground,
                LeavingBackground,
                Destroy
            };

            private readonly Step[] _scenario;
            private int _currentStep = 0;
            private bool _disposed = false;

            public LifecycleEventsListener(Step[] scenario)
            {
                _scenario = (Step[])scenario.Clone();
            }

            public void OnEnteredBackground()
            {
                Advance(Step.EnteredBackground);
            }

            public void OnLeavingBackground()
            {
                Advance(Step.LeavingBackground);
            }

            public void OnSuspend()
            {
                Advance(Step.Suspend);
            }

            public void OnResume()
            {
                Advance(Step.Resume);
            }

            public void OnDestroy()
            {
                Advance(Step.Destroy);
            }

            public void Dispose()
            {
                if (!_disposed)
                {
                    Assert.IsFalse(_currentStep != _scenario.Length, "Too few steps");
                    _disposed = true;
                }
            }

            private void Advance(Step step)
            {
                if (_disposed) return;

                Assert.IsFalse(_currentStep >= _scenario.Length, "Too many steps");
                Assert.AreEqual(_scenario[_currentStep], step);
                _currentStep++;
            }
        }

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
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager());

            await AssertEx.ThrowsAsync<ArgumentNullException>(
                async () => await DispatcherHelpers.CallOnDispatcherAsync(() =>
                    manager.AttachMeasuredRootViewAsync(null)),
                ex => Assert.AreEqual("rootView", ex.ParamName));

            await AssertEx.ThrowsAsync<ArgumentNullException>(
                async () => await DispatcherHelpers.CallOnDispatcherAsync(() =>
                    manager.CreateAllViewManagers(null)),
                ex => Assert.AreEqual("reactContext", ex.ParamName));

            await AssertEx.ThrowsAsync<ArgumentNullException>(
                async () => await DispatcherHelpers.CallOnDispatcherAsync(() =>
                    manager.DetachRootViewAsync(null)),
                ex => Assert.AreEqual("rootView", ex.ParamName));

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateReactContextAsync()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(
                () => manager.CreateReactContextAsync(CancellationToken.None));

            Assert.AreEqual(jsBundleFile, manager.SourceUrl);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_CreateReactContextAsync_EnsuresOneCall()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var caught = false;
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
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

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_RecreateReactContextAsync()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

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

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_RecreateReactContextAsync_EnsuresCalledOnce()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var caught = false;
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
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

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_OnBackPressed_NoContext()
        {
            var waitHandle = new AutoResetEvent(false);
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager());

            await DispatcherHelpers.RunOnDispatcherAsync(() =>
            {
                manager.OnResume(() => waitHandle.Set());
                manager.OnBackPressed();
            });

            Assert.IsTrue(waitHandle.WaitOne());

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_DisposeAsync_WhileBusy()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

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
                await DisposeInstanceManager(manager);
                ;
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
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var cancellationTokenSource = new CancellationTokenSource();
            cancellationTokenSource.Cancel();
            await AssertEx.ThrowsAsync<OperationCanceledException>(
                async () => await DispatcherHelpers.CallOnDispatcherAsync(() =>
                    manager.CreateReactContextAsync(cancellationTokenSource.Token)),
                ex => Assert.AreEqual(cancellationTokenSource.Token, ex.CancellationToken));

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Unfinished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var initialContextTask = DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.GetReactContextAsync(CancellationToken.None));
            var initialContext = await initialContextTask;

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }


        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Finished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var initialContext = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.GetReactContextAsync(CancellationToken.None));

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetReactContextAsync_Fail()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

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

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_TryGetReactContextAsync_Unfinished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var initialContextTask = DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.TryGetReactContextAsync(CancellationToken.None));
            var initialContext = await initialContextTask;

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_TryGetReactContextAsync_Finished()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var initialContext = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.CreateReactContextAsync(CancellationToken.None));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.TryGetReactContextAsync(CancellationToken.None));

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_TryGetReactContextAsync_Fail()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));
            var context = await DispatcherHelpers.CallOnDispatcherAsync(async () =>
                await manager.TryGetReactContextAsync(CancellationToken.None));

            Assert.IsNull(context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetOrCreateReactContextAsync_Create()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var reactContext = await DispatcherHelpers.CallOnDispatcherAsync(() => manager.GetOrCreateReactContextAsync(CancellationToken.None));
            Assert.IsNotNull(reactContext);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_GetOrCreateReactContextAsync_Get()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile));

            var initialContext = default(ReactContext);
            var context = default(ReactContext);
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var initialContextTask = manager.CreateReactContextAsync(CancellationToken.None);
                context = await manager.GetOrCreateReactContextAsync(CancellationToken.None);
                initialContext = await initialContextTask;
            });

            Assert.AreSame(initialContext, context);

            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_Simple()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.BeforeCreate));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.Resume,
                LifecycleEventsListener.Step.LeavingBackground,
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.LeavingBackground,
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.Suspend,
                LifecycleEventsListener.Step.Resume,
                LifecycleEventsListener.Step.LeavingBackground,
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.Suspend,
            });
            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                manager.OnResume(null);
                manager.OnLeavingBackground();
                manager.OnEnteredBackground();
                manager.OnLeavingBackground();
                manager.OnEnteredBackground();
                manager.OnSuspend();
                manager.OnResume(null);
                manager.OnLeavingBackground();
                manager.OnEnteredBackground();
                manager.OnSuspend();

                context.RemoveLifecycleEventListener(listener);
                context.RemoveBackgroundEventListener(listener);
            });

            listener.Dispose();
            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_Dispose_From_Foreground()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.Foreground));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.Suspend,
                LifecycleEventsListener.Step.Destroy,
            });

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                await DisposeInstanceManager(manager);
            });

            listener.Dispose();
            // manager has been disposed already
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_NoForeground()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.BeforeCreate));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.Resume,
                LifecycleEventsListener.Step.Suspend,
            });

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                manager.OnResume(null);
                manager.OnSuspend();

                context.RemoveLifecycleEventListener(listener);
                context.RemoveBackgroundEventListener(listener);
            });

            listener.Dispose();
            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_StartSuspended()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.BeforeCreate));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.Resume,
                LifecycleEventsListener.Step.Suspend,
            });

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                manager.OnSuspend();

                context.RemoveLifecycleEventListener(listener);
                context.RemoveBackgroundEventListener(listener);
            });

            listener.Dispose();
            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_Missing_Background()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.Foreground));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.Suspend,
            });

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                manager.OnSuspend();

                context.RemoveLifecycleEventListener(listener);
                context.RemoveBackgroundEventListener(listener);
            });

            listener.Dispose();
            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        [TestMethod]
        public async Task ReactInstanceManager_Lifecycle_Cleanup_Old_Context()
        {
            var jsBundleFile = "ms-appx:///Resources/test.js";
            ReactInstanceManager manager = null;
            await DispatcherHelpers.CallOnDispatcherAsync(() => manager = CreateReactInstanceManager(jsBundleFile, LifecycleState.Foreground));

            var listener = new LifecycleEventsListener(new LifecycleEventsListener.Step[]
            {
                LifecycleEventsListener.Step.EnteredBackground,
                LifecycleEventsListener.Step.Suspend,
            });

            await DispatcherHelpers.CallOnDispatcherAsync(async () =>
            {
                var context = await manager.CreateReactContextAsync(CancellationToken.None);
                context.AddBackgroundEventListener(listener);
                context.AddLifecycleEventListener(listener);

                var newContext = await manager.RecreateReactContextAsync(CancellationToken.None);
            });
                
            listener.Dispose();
            await DispatcherHelpers.CallOnDispatcherAsync(async () => await DisposeInstanceManager(manager));
        }

        private static ReactInstanceManager CreateReactInstanceManager()
        {
            return CreateReactInstanceManager("ms-appx:///Resources/main.jsbundle");
        }

        private static ReactInstanceManager CreateReactInstanceManager(string jsBundleFile, LifecycleState initialLifecycleState = LifecycleState.Foreground)
        {
            ReactNative.Bridge.DispatcherHelpers.Initialize();
            ReactChoreographer.Initialize();

            return new ReactInstanceManagerBuilder
            {
                InitialLifecycleState = initialLifecycleState,
                JavaScriptBundleFile = jsBundleFile,
            }.Build();
        }

        private static async Task DisposeInstanceManager(ReactInstanceManager manager)
        {
            await manager.DisposeAsync();

            // Go back to the initial state as set by the host test app
            ReactNative.Bridge.DispatcherHelpers.Initialize();
        }
    }
}
