// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Chakra.Executor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class ReactBridgeTests
    {
        private JToken _response;
        private List<(int moduleId, int methodId, JArray args)> _receivedCallbacks;
        private AutoResetEvent _eventHandler;
        private MockReactCallback _mockReactCallback;
        private MockJavaScriptExecutor _mockJavaScriptExecutor;
        private IActionQueue _nativeModulesQueueThread;
        private ReactBridge _reactBridge;

        [SetUp]
        public void SetUp()
        {
            _receivedCallbacks = new List<(int moduleId, int methodId, JArray args)>();
            _eventHandler = new AutoResetEvent(false);

            _mockReactCallback = new MockReactCallback
            {
                InvokeHandler = (moduleId, methodId, args) => _receivedCallbacks.Add((moduleId, methodId, args)),
                OnBatchCompleteHandler = () => _eventHandler.Set(),
            };

            _mockJavaScriptExecutor = new MockJavaScriptExecutor
            {
                OnCallFunctionReturnFlushedQueue = (module, method, args) => _response
            };

            _nativeModulesQueueThread = CreateNativeModulesThread();

            _reactBridge = new ReactBridge(_mockJavaScriptExecutor, _mockReactCallback, _nativeModulesQueueThread);
        }

        [Test]
        public async Task ReactBridge_Ctor_ArgumentChecks()
        {
            await JavaScriptHelpers.Run((executor, jsQueueThread) =>
            {
                using (_nativeModulesQueueThread)
                {
                    var reactCallback = new MockReactCallback();

                    Assert.That(
                        () => new ReactBridge(null, reactCallback, _nativeModulesQueueThread),
                        Throws.ArgumentNullException.With.Property("ParamName").EqualTo("executor")
                    );

                    Assert.That(
                        () => new ReactBridge(executor, null, _nativeModulesQueueThread),
                        Throws.ArgumentNullException.With.Property("ParamName").EqualTo("reactCallback")
                    );

                    Assert.That(
                        () => new ReactBridge(executor, reactCallback, null),
                        Throws.ArgumentNullException.With.Property("ParamName").EqualTo("nativeModulesQueueThread")
                    );
                }
            });
        }

        [Test]
        public async Task ReactBridge_Method_ArgumentChecks()
        {
            await JavaScriptHelpers.Run((executor, jsQueueThread) =>
            {
                using (_nativeModulesQueueThread)
                {
                    var reactCallback = new MockReactCallback();
                    var bridge = new ReactBridge(executor, reactCallback, _nativeModulesQueueThread);

                    Assert.That(
                        () => bridge.SetGlobalVariable(null, null),
                        Throws.ArgumentNullException.With.Property("ParamName").EqualTo("propertyName")
                    );
                }
            });
        }

        [Test]
        public async Task ReactBridge_CallFunction()
        {
            await JavaScriptHelpers.Run(async (executor, jsQueueThread) =>
            {
                using (_nativeModulesQueueThread)
                {
                    var bridge = new ReactBridge(executor, new MockReactCallback(), _nativeModulesQueueThread);
                    var token = await jsQueueThread.RunAsync(() =>
                    {
                        bridge.CallFunction("module", "method", new JArray());
                        return executor.GetGlobalVariable("FunctionCalls");
                    });

                    var expected = new JArray
                    {
                        new JArray
                        {
                            "module",
                            "method",
                            new JArray(),
                        },
                    };

                    Assert.That(expected.ToString(Formatting.None), Is.EqualTo(token.ToString(Formatting.None)));
                }
            });
        }

        [Test]
        public async Task ReactBridge_InvokeCallback()
        {
            await JavaScriptHelpers.Run(async (executor, jsQueueThread) =>
            {
                using (var nativeThread = new ActionQueue(ex => { Assert.Fail(); }))
                {
                    var bridge = new ReactBridge(executor, new MockReactCallback(), nativeThread);
                    var token = await jsQueueThread.RunAsync(() =>
                    {
                        bridge.InvokeCallback(1, new JArray());
                        return executor.GetGlobalVariable("CallbackCalls");
                    });

                    var expected = new JArray
                    {
                        new JArray
                        {
                            1,
                            new JArray(),
                        },
                    };

                    Assert.That(expected.ToString(Formatting.None), Is.EqualTo(token.ToString(Formatting.None)));
                }
            });
        }

        [Test]
        public void ReactBridge_ReactCallbacks()
        {
            var jsonResponse = JArray.Parse("[[42,17],[16,22],[[],[\"foo\"]]]");
            var callbacks = new List<Tuple<int, int, JArray>>();
            var eventHandler = new AutoResetEvent(false);
            var callback = new MockReactCallback
            {
                InvokeHandler = (moduleId, methodId, args) => callbacks.Add(Tuple.Create(moduleId, methodId, args)),
                OnBatchCompleteHandler = () => eventHandler.Set(),
            };

            var executor = new MockJavaScriptExecutor
            {
                OnCallFunctionReturnFlushedQueue = (module, method, args) =>
                {
                    Assert.AreEqual(module, "module");
                    Assert.AreEqual(method, "method");
                    return jsonResponse;
                }
            };

            using (_nativeModulesQueueThread)
            {
                var bridge = new ReactBridge(executor, callback, _nativeModulesQueueThread);
                bridge.CallFunction("module", "method", new JArray());

                Assert.IsTrue(eventHandler.WaitOne());
            }

            Assert.AreEqual(2, callbacks.Count);

            Assert.AreEqual(42, callbacks[0].Item1);
            Assert.AreEqual(16, callbacks[0].Item2);
            Assert.AreEqual(0, callbacks[0].Item3.Count);

            Assert.AreEqual(17, callbacks[1].Item1);
            Assert.AreEqual(22, callbacks[1].Item2);
            Assert.AreEqual(1, callbacks[1].Item3.Count);
            Assert.AreEqual("foo", callbacks[1].Item3[0].Value<string>());
        }

        [Test]
        public void ReactBridge_ValidJavaScriptResponse()
        {
            var responses = new[]
            {
                JToken.Parse("null"),
                JToken.Parse("undefined"),
                JArray.Parse("[[],[],[]]"),
                JArray.Parse("[[1],[1],[[]]]"),
                JArray.Parse("[[1],[1],[[1,2,3]], 42]"),
            };

            var n = responses.Length;
            using (_nativeModulesQueueThread)
            {
                var count = 0;
                var executor = new MockJavaScriptExecutor
                {
                    OnCallFunctionReturnFlushedQueue = (module, method, args) =>
                    {
                        return responses[count++];
                    }
                };

                var bridge = new ReactBridge(executor, new MockReactCallback(), _nativeModulesQueueThread);

                for (var i = 0; i < n; ++i)
                {
                    bridge.CallFunction("module", "method", new JArray());
                }

                Assert.AreEqual(n, count);
            }
        }

        [Test]
        public void SingleReturnCallNoArgs()
        {
            const string jsText = "[[7],[3],[[]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args.Count, Is.EqualTo(0));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void StringReturn()
        {
            const string jsText = "[[0],[0],[[\"foobar\"]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.String));
            Assert.That(_receivedCallbacks[0].args[0].Value<string>(), Is.EqualTo("foobar"));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void NumberReturn()
        {
            const string jsText = "[[0],[0],[[42.16]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.Float));
            Assert.That(_receivedCallbacks[0].args[0].Value<double>(), Is.EqualTo(42.16));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void BoolReturn()
        {
            const string jsText = "[[0],[0],[[false]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.Boolean));
            Assert.That(_receivedCallbacks[0].args[0].Value<bool>(), Is.EqualTo(false));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void NullReturn()
        {
            const string jsText = "[[0],[0],[[null]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.Null));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void MapReturn()
        {
            const string jsText = "[[0],[0],[[{\"foo\": \"hello\", \"bar\": 4.2, \"baz\": true}]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.Object));
            Assert.That(_receivedCallbacks[0].args[0]["foo"].Value<string>(), Is.EqualTo("hello"));
            Assert.That(_receivedCallbacks[0].args[0]["bar"].Value<double>(), Is.EqualTo(4.2));
            Assert.That(_receivedCallbacks[0].args[0]["baz"].Value<bool>(), Is.EqualTo(true));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void ArrayReturn()
        {
            const string jsText = "[[0],[0],[[[\"foo\", 4.2, false]]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.Array));
            Assert.That(_receivedCallbacks[0].args[0].Count(), Is.EqualTo(3));
            Assert.That(_receivedCallbacks[0].args[0][0].Value<string>, Is.EqualTo("foo"));
            Assert.That(_receivedCallbacks[0].args[0][1].Value<double>(), Is.EqualTo(4.2));
            Assert.That(_receivedCallbacks[0].args[0][2].Value<bool>(), Is.False);
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void ReturnMultipleParams()
        {
            const string jsText = "[[0],[0],[[\"foo\", 14, null, false]]]";
            _response = JToken.Parse(jsText);
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(1));
            Assert.That(_receivedCallbacks[0].args.Count, Is.EqualTo(4));
            Assert.That(_receivedCallbacks[0].args[0].Type, Is.EqualTo(JTokenType.String));
            Assert.That(_receivedCallbacks[0].args[1].Type, Is.EqualTo(JTokenType.Integer));
            Assert.That(_receivedCallbacks[0].args[2].Type, Is.EqualTo(JTokenType.Null));
            Assert.That(_receivedCallbacks[0].args[3].Type, Is.EqualTo(JTokenType.Boolean));
            Assert.That(_receivedCallbacks[0].args[0].Value<string>, Is.EqualTo("foo"));
            Assert.That(_receivedCallbacks[0].args[1].Value<int>(), Is.EqualTo(14));
            Assert.That(_receivedCallbacks[0].args[3].Value<bool>(), Is.False);
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }


        [Test]
        public void ParseTwoCalls()
        {
            const string jsText = "[[0,0],[1,1],[[],[]]]";
            _response = JToken.Parse(jsText);

            CallFunctionOnReactBridge();

            Assert.That(_receivedCallbacks.Count, Is.EqualTo(2));
            Assert.That(_mockReactCallback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public void ReactBridge_InvalidJavaScriptResponse()
        {
            var responses = new[]
            {
                JArray.Parse("[null,[],[]]"),
                JArray.Parse("[[],null,[]]"),
                JArray.Parse("[[],[],null]"),
                JArray.Parse("[[42],[],[]]"),
                JArray.Parse("[[],[42],[]]"),
                JArray.Parse("[[],[],[42]]"),
                JArray.Parse("[[42],[42],[]]"),
                JArray.Parse("[[42],[42],[[],[]]]"),
                JArray.Parse("[[]]"),
                JArray.Parse("[[],[]]"),
                JObject.Parse("{}"),
                JToken.Parse("0"),
            };

            var n = responses.Length;
            using (_nativeModulesQueueThread)
            {
                var count = 0;
                var executor = new MockJavaScriptExecutor
                {
                    OnCallFunctionReturnFlushedQueue = (module, method, args) =>
                    {
                        return responses[count++];
                    }
                };

                var callback = new MockReactCallback();
                var bridge = new ReactBridge(executor, callback, _nativeModulesQueueThread);

                for (var i = 0; i < n; ++i)
                {
                    Assert.That(
                        () => bridge.CallFunction("module", "method", new JArray()),
                        Throws.InvalidOperationException
                    );
                }

                Assert.AreEqual(n, count);
                Assert.That(callback.PendingJavaScriptCalls, Is.Zero);
            }
        }

        [Test]
        public async Task ReactBridge_SyncCallback()
        {
            var jsFactories = new Func<IJavaScriptExecutor>[]
            {
                () => new ChakraJavaScriptExecutor(),
#if WINDOWS_UWP
                () => new NativeJavaScriptExecutor(),
#endif
            };

            var moduleId = 42;
            var methodId = 7;
            var called = 0;
            var countdownEvent = new CountdownEvent(1);
            var callback = new MockReactCallback
            {
                InvokeSyncHandler = (mod, met, arg) =>
                {
                    Assert.AreEqual(moduleId, mod);
                    Assert.AreEqual(methodId, met);
                    Assert.AreEqual(0, arg.Count);
                    ++called;
                    return JValue.CreateNull();
                },
                OnBatchCompleteHandler = () => countdownEvent.Signal(),
            };

            foreach (var jsFactory in jsFactories)
            {
                await JavaScriptHelpers.Run(async (executor, jsQueueThread) =>
                {
                    using (_nativeModulesQueueThread)
                    {
                        var bridge = new ReactBridge(executor, callback, _nativeModulesQueueThread);
                        await jsQueueThread.RunAsync(() =>
                        {
                            bridge.CallFunction("SyncModule", "test", new JArray { 42, 7, new JArray() });
                        });
                    }
                },
                jsFactory,
                @"Resources/sync.js");
            }



            Assert.AreEqual(1, called);
            // FIXME: this test is flaky, but adding synchronization makes it wait forever (unlike below)
            // await Task.Run(new Action(countdownEvent.Wait));
            // Assert.That(callback.PendingJavaScriptCalls, Is.Zero);
        }

        [Test]
        public async Task ReactBridge_FlushQueueImmediate()
        {
            var jsFactories = new Func<IJavaScriptExecutor>[]
            {
                () => new ChakraJavaScriptExecutor(),
#if WINDOWS_UWP
                () => new NativeJavaScriptExecutor(),
#endif
            };

            foreach (var jsFactory in jsFactories)
            {
                await JavaScriptHelpers.Run(async (executor, jsQueueThread) =>
                {
                    using (_nativeModulesQueueThread)
                    {
                        var moduleId = 42;
                        var methodId = 7;
                        var called = 0;
                        var countdownEvent = new CountdownEvent(1);
                        var callback = new MockReactCallback
                        {
                            InvokeHandler = (mod, met, arg) =>
                            {
                                Assert.AreEqual(moduleId, mod);
                                Assert.AreEqual(methodId, met);
                                Assert.AreEqual(1, arg.Count);
                                Assert.AreEqual("foo", arg[0].Value<string>());
                                ++called;
                            },
                            OnBatchCompleteHandler = () => countdownEvent.Signal(),
                        };

                        var bridge = new ReactBridge(executor, callback, _nativeModulesQueueThread);
                        await jsQueueThread.RunAsync(() =>
                        {
                            bridge.CallFunction("FlushQueueImmediateModule", "test", new JArray { 10, new JArray { new JArray { 42 }, new JArray { 7 }, new JArray { new JArray { "foo" } } } });
                        });

                        // wait for `OnBatchComplete` in background
                        // so we don't block native module thread
                        await Task.Run(new Action(countdownEvent.Wait));

                        Assert.AreEqual(10, called);
                        Assert.That(callback.PendingJavaScriptCalls, Is.Zero);
                    }
                },
                jsFactory,
                @"Resources/immediate.js");
            }
        }

        private void CallFunctionOnReactBridge()
        {
            using (_nativeModulesQueueThread)
            {
                _reactBridge.CallFunction("module", "method", new JArray());

                Assert.That(_eventHandler.WaitOne());
            }
        }

        private static IActionQueue CreateNativeModulesThread()
        {
            return CreateNativeModulesThread(ex => Assert.Fail(ex.ToString()));
        }

        private static IActionQueue CreateNativeModulesThread(Action<Exception> exceptionHandler)
        {
            return new ActionQueue(exceptionHandler);
        }
    }
}
