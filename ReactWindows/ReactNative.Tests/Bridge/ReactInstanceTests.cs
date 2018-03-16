// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Tests.Bridge
{
    [TestClass]
    public class ReactInstanceTests
    {
        [TestMethod]
        public async Task ReactInstance_GetModules()
        {
            var module = new TestNativeModule();
            var reactContext = new ReactContext();

            var registry = new NativeModuleRegistry.Builder(reactContext)
                .Add(module)
                .Build();

            var executor = new MockJavaScriptExecutor
            {
                OnCallFunctionReturnFlushedQueue = (_, __, ___) => JValue.CreateNull(),
                OnFlushQueue = () => JValue.CreateNull(),
                OnInvokeCallbackAndReturnFlushedQueue = (_, __) => JValue.CreateNull()
            };

            var builder = new ReactInstance.Builder()
            {
                QueueConfiguration = TestReactQueueConfiguration.Create(_ => { }),
                Registry = registry,
                JavaScriptExecutorFactory = () => executor,
                BundleLoader = JavaScriptBundleLoader.CreateFileLoader("ms-appx:///Resources/test.js"),
            };

            var instance = await DispatcherHelpers.CallOnDispatcherAsync(() => builder.Build());
            reactContext.InitializeWithInstance(instance);

            var actualModule = instance.GetNativeModule<TestNativeModule>();
            Assert.AreSame(module, actualModule);

            var firstJSModule = instance.GetJavaScriptModule<TestJavaScriptModule>();
            var secondJSModule = instance.GetJavaScriptModule<TestJavaScriptModule>();
            Assert.AreSame(firstJSModule, secondJSModule);

            await DispatcherHelpers.CallOnDispatcherAsync(instance.DisposeAsync);
        }

        [TestMethod]
        public async Task ReactInstance_Initialize_Dispose()
        {
            var mre = new ManualResetEvent(false);
            var module = new TestNativeModule
            {
                OnInitialized = () => mre.Set(),
            };

            var reactContext = new ReactContext();
            var registry = new NativeModuleRegistry.Builder(reactContext)
                .Add(module)
                .Build();

            var executor = new MockJavaScriptExecutor
            {
                OnCallFunctionReturnFlushedQueue = (_, __, ___) => JValue.CreateNull(),
                OnFlushQueue = () => JValue.CreateNull(),
                OnInvokeCallbackAndReturnFlushedQueue = (_, __) => JValue.CreateNull()
            };
            var builder = new ReactInstance.Builder()
            {
                QueueConfiguration = TestReactQueueConfiguration.Create(_ => { }),
                Registry = registry,
                JavaScriptExecutorFactory = () => executor,
                BundleLoader = JavaScriptBundleLoader.CreateFileLoader("ms-appx:///Resources/test.js"),
            };

            var instance = await DispatcherHelpers.CallOnDispatcherAsync(() => builder.Build());
            reactContext.InitializeWithInstance(instance);
            await DispatcherHelpers.RunOnDispatcherAsync(() => instance.Initialize());

            var caught = false;
            await DispatcherHelpers.RunOnDispatcherAsync(() =>
            {
                try
                {
                    instance.Initialize();
                }
                catch (InvalidOperationException)
                {
                    caught = true;
                }
            });

            Assert.IsTrue(caught);
            mre.WaitOne();
            Assert.AreEqual(1, module.InitializeCalls);

            await DispatcherHelpers.CallOnDispatcherAsync(instance.DisposeAsync);
            Assert.AreEqual(1, module.OnReactInstanceDisposeCalls);

            // Dispose is idempotent
            await DispatcherHelpers.CallOnDispatcherAsync(instance.DisposeAsync);
            Assert.AreEqual(1, module.OnReactInstanceDisposeCalls);

            Assert.IsTrue(instance.IsDisposed);
        }

        [TestMethod]
        public async Task ReactInstance_ExceptionHandled_DoesNotDispose()
        {
            var eventHandler = new AutoResetEvent(false);
            var module = new OnDisposeNativeModule(() => eventHandler.Set());
            var registry = new NativeModuleRegistry.Builder(new ReactContext())
                .Add(module)
                .Build();

            var executor = new MockJavaScriptExecutor
            {
                OnCallFunctionReturnFlushedQueue = (_, __, ___) => JValue.CreateNull(),
                OnFlushQueue = () => JValue.CreateNull(),
                OnInvokeCallbackAndReturnFlushedQueue = (_, __) => JValue.CreateNull()
            };

            var exception = new Exception();
            var tcs = new TaskCompletionSource<Exception>();
            var handler = new Action<Exception>(ex =>
            {
                Task.Run(() => tcs.SetResult(ex));
            });

            var builder = new ReactInstance.Builder()
            {
                QueueConfiguration = TestReactQueueConfiguration.Create(handler),
                Registry = registry,
                JavaScriptExecutorFactory = () => executor,
                BundleLoader = JavaScriptBundleLoader.CreateFileLoader("ms-appx:///Resources/test.js"),
            };

            var instance = await DispatcherHelpers.CallOnDispatcherAsync(() => builder.Build());
            instance.QueueConfiguration.JavaScriptQueue.Dispatch(() =>
            {
                throw exception;
            });

            var actualException = await tcs.Task;
            Assert.AreSame(exception, actualException);

            Assert.IsFalse(eventHandler.WaitOne(500));
            Assert.IsFalse(instance.IsDisposed);
        }

        class TestNativeModule : NativeModuleBase
        {
            public Action OnInitialized
            {
                get;
                set;
            }

            public int InitializeCalls
            {
                get;
                set;
            }

            public int OnReactInstanceDisposeCalls
            {
                get;
                set;
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            public override void Initialize()
            {
                InitializeCalls++;
                OnInitialized?.Invoke();
            }

            public override void OnReactInstanceDispose()
            {
                OnReactInstanceDisposeCalls++;
            }
        }

        class OnDisposeNativeModule : NativeModuleBase
        {
            private readonly Action _onDispose;

            public OnDisposeNativeModule(Action onDispose)
            {
                _onDispose = onDispose;
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            public override void OnReactInstanceDispose()
            {
                _onDispose();
            }
        }

        class TestJavaScriptModule : JavaScriptModuleBase
        {
        }
    }
}
