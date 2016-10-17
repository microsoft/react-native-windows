using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class NativeModuleBaseTests
    {
        [Test]
        public void NativeModuleBase_ReactMethod_ThrowsNotSupported()
        {
            var actions = new Action[]
            {
                () => new MethodOverloadNotSupportedNativeModule(),
                () => new ReturnTypeNotSupportedNativeModule(),
                () => new CallbackNotSupportedNativeModule(),
                () => new CallbackNotSupportedNativeModule2(),
                () => new PromiseNotSupportedNativeModule(),
                () => new AsyncCallbackNotSupportedNativeModule(),
                () => new AsyncPromiseNotSupportedNativeModule(),
            };

            foreach (var action in actions)
            {
                Assert.That(() => { action(); }, Throws.Exception.With.TypeOf<NotSupportedException>());
            }
        }

        [Test]
        public void NativeModuleBase_ReactMethod_Async_ThrowsNotImplemented()
        {
            Assert.That(() => new AsyncNotImplementedNativeModule(), Throws.Exception.With.TypeOf<NotImplementedException>());
        }

        [Test]
        public void NativeModuleBase_Invocation_ArgumentNull()
        {
            var testModule = new TestNativeModule();

            testModule.Initialize();

            var reactInstance = new MockReactInstance();
            Assert.That(
                () => { testModule.Methods[nameof(TestNativeModule.Foo)].Invoke(null, new JArray()); }, 
                Throws.ArgumentNullException.With.Property("ParamName").EqualTo("reactInstance")
            );

            Assert.That(
                () => { testModule.Methods[nameof(TestNativeModule.Foo)].Invoke(reactInstance, null); },
                Throws.ArgumentNullException.With.Property("ParamName").EqualTo("jsArguments")
            );
        }

        [Test]
        public void NativeModuleBase_Invocation_ArgumentInvalidCount()
        {
            var testModule = new TestNativeModule();

            testModule.Initialize();

            var reactInstance = new MockReactInstance();
            Assert.That(
                () => { testModule.Methods[nameof(TestNativeModule.Bar)].Invoke(reactInstance, new JArray()); },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("jsArguments")
            );
        }

        [Test]
        public void NativeModuleBase_Invocation_ArgumentConversionException()
        {
            var testModule = new TestNativeModule();

            testModule.Initialize();

            var reactInstance = new MockReactInstance();
            Assert.That(
                () => { testModule.Methods[nameof(TestNativeModule.Bar)].Invoke(reactInstance, JArray.FromObject(new[] { default(object) })); },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("arguments")
            );
        }

        [Test]
        public void NativeModuleBase_Invocation()
        {
            var fooCount = 0;
            var barSum = 0;
            var testModule = new TestNativeModule(() => fooCount++, x => barSum += x);

            testModule.Initialize();

            Assert.That(testModule.Methods.Count, Is.EqualTo(2));

            var reactInstance = new MockReactInstance();
            testModule.Methods[nameof(TestNativeModule.Foo)].Invoke(reactInstance, new JArray());
            testModule.Methods[nameof(TestNativeModule.Foo)].Invoke(reactInstance, new JArray());
            Assert.That(fooCount, Is.EqualTo(2));

            testModule.Methods[nameof(TestNativeModule.Bar)].Invoke(reactInstance, JArray.FromObject(new[] { 42 }));
            testModule.Methods[nameof(TestNativeModule.Bar)].Invoke(reactInstance, JArray.FromObject(new[] { 17 }));
            Assert.That(barSum, Is.EqualTo(59));
        }

        [Test]
        public void NativeModuleBase_Invocation_Callbacks()
        {
            var callbackArgs = new object[] { 1, 2, 3 };
            var module = new CallbackNativeModule(callbackArgs);
            module.Initialize();

            var id = default(int);
            var args = default(List<int>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<int>>();
            });

            module.Methods[nameof(CallbackNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { 42 }));
            Assert.That(id, Is.EqualTo(42));
            Assert.That(args.Cast<object>(), Is.EqualTo(callbackArgs));
        }

        [Test]
        public void NativeModuleBase_Invocation_Callbacks_InvalidArgumentThrows()
        {
            var callbackArgs = new object[] { 1, 2, 3 };
            var module = new CallbackNativeModule(callbackArgs);
            module.Initialize();

            var id = default(int);
            var args = default(List<int>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<int>>();
            });

            Assert.That(
                () => { module.Methods[nameof(CallbackNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { default(object) })); },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("arguments")
            );
        }

        [Test]
        public void NativeModuleBase_Invocation_Callbacks_NullCallback()
        {
            var module = new CallbackNativeModule(null);
            module.Initialize();

            var id = default(int);
            var args = default(List<int>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<int>>();
            });

            module.Methods[nameof(CallbackNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { 42 }));
            Assert.That(args.Count, Is.EqualTo(0));
        }

        [Test]
        public void NativeModuleBase_Invocation_Promises_Resolve()
        {
            var module = new PromiseNativeModule(() => 17);
            module.Initialize();

            var id = default(int);
            var args = default(List<int>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<int>>();
            });

            module.Methods[nameof(PromiseNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { 42, 43 }));
            Assert.That(id, Is.EqualTo(42));
            Assert.That(args, Is.EqualTo(new[] { 17 }));
        }

        [Test]
        public void NativeModuleBase_CompiledDelegateFactory_Perf()
        {
            var module = new PerfNativeModule(CompiledReactDelegateFactory.Instance);
            var reactInstance = new MockReactInstance();
            var args = JArray.FromObject(new[] { 42 });

            module.Initialize();

            var n = 100000;
            for (var i = 0; i < n; ++i)
            {
                module.Methods[nameof(PerfNativeModule.Foo)].Invoke(reactInstance, args);
            }

            // FIXME: no explicit assertions
        }

        [Test]
        public void NativeModuleBase_Invocation_Promises_InvalidArgumentThrows()
        {
            var module = new PromiseNativeModule(() => 17);
            module.Initialize();

            var id = default(int);
            var args = default(List<int>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<int>>();
            });

            Assert.That(
                () =>
                {
                    module.Methods[nameof(PromiseNativeModule.Foo)].Invoke(reactInstance,
                        JArray.FromObject(new[] { default(object), 43 }));
                },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("arguments"));

            Assert.That(
                () =>
                {
                    module.Methods[nameof(PromiseNativeModule.Foo)].Invoke(reactInstance,
                        JArray.FromObject(new[] { 42, default(object) }));
                },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("arguments"));
        }

        [Test]
        public void NativeModuleBase_Invocation_Promises_IncorrectArgumentCount()
        {
            var module = new PromiseNativeModule(() => null);
            module.Initialize();

            var id = default(int);
            var args = default(List<object>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<object>>();
            });

            Assert.That(
                () =>
                {
                    module.Methods[nameof(PromiseNativeModule.Foo)].Invoke(reactInstance,
                        JArray.FromObject(new[] { 42 }));
                },
                Throws.Exception.With.TypeOf<NativeArgumentsParseException>()
                    .And.Property("ParamName")
                    .EqualTo("jsArguments"));
        }

        [Test]
        public void NativeModuleBase_Invocation_Promises_Reject()
        {
            var expectedMessage = "Foo bar baz";
            var exception = new Exception(expectedMessage);
            var module = new PromiseNativeModule(() => { throw exception; });
            module.Initialize();

            var id = default(int);
            var args = default(Dictionary<string, string>[]);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<Dictionary<string, string>[]>();
            });

            module.Methods[nameof(CallbackNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { 42, 43 }));
            Assert.AreEqual(43, id);
            Assert.AreEqual(1, args.Length);
            var d = args[0];
            Assert.AreEqual(3, d.Count);
            var actualMessage = default(string);
            Assert.IsTrue(d.TryGetValue("message", out actualMessage));
            Assert.AreEqual(expectedMessage, actualMessage);
        }

        [Test]
        public void NativeModuleBase_Invocation_Promises_NullCallback()
        {
            var module = new PromiseNativeModule(() => null);
            module.Initialize();

            var id = default(int);
            var args = default(List<object>);

            var reactInstance = new MockReactInstance((i, a) =>
            {
                id = i;
                args = a.ToObject<List<object>>();
            });

            module.Methods[nameof(PromiseNativeModule.Foo)].Invoke(reactInstance, JArray.FromObject(new[] { 42, 43 }));
            Assert.AreEqual(1, args.Count);
            Assert.IsNull(args[0]);
        }

        [Test]
        public void NativeModuleBase_ReflectionDelegateFactory_Perf()
        {
            var module = new PerfNativeModule(ReflectionReactDelegateFactory.Instance);
            var reactInstance = new MockReactInstance();
            var args = JArray.FromObject(new[] { 42 });

            module.Initialize();

            var n = 100000;
            for (var i = 0; i < n; ++i)
            {
                module.Methods[nameof(PerfNativeModule.Foo)].Invoke(reactInstance, args);
            }
        }

        class MethodOverloadNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo()
            {
            }

            [ReactMethod]
            public void Foo(int x)
            {
            }
        }

        class ReturnTypeNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public int Foo() { return 0; }
        }
        
        class CallbackNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(ICallback foo, int bar, string qux) { }
        }

        class CallbackNotSupportedNativeModule2 : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(ICallback bar, int foo) { }
        }

        class PromiseNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(IPromise promise, int foo) { }
        }

        class AsyncCallbackNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public Task Foo(ICallback callback)
            {
                return Task.CompletedTask;
            }
        }

        class AsyncPromiseNotSupportedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public Task Foo(IPromise promise)
            {
                return Task.CompletedTask;
            }
        }

        class AsyncNotImplementedNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public Task Foo()
            {
                return Task.CompletedTask;
            }
        }

        class TestNativeModule : NativeModuleBase
        {
            private readonly Action _onFoo;
            private readonly Action<int> _onBar;

            public TestNativeModule()
                : this(() => { }, _ => { })
            {
            }

            public TestNativeModule(Action onFoo, Action<int> onBar)
            {
                _onFoo = onFoo;
                _onBar = onBar;
            }

            public override string Name
            {
                get
                {
                    return "Foo";
                }
            }

            [ReactMethod]
            public void Foo()
            {
                _onFoo();
            }

            [ReactMethod]
            public void Bar(int x)
            {
                _onBar(x);
            }
        }

        class CallbackNativeModule : NativeModuleBase
        {
            private readonly object[] _callbackArgs;

            public CallbackNativeModule()
                : this(null)
            {
            }

            public CallbackNativeModule(object[] callbackArgs)
            {
                _callbackArgs = callbackArgs;
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(ICallback callback)
            {
                callback.Invoke(_callbackArgs);
            }
        }

        class PromiseNativeModule : NativeModuleBase
        {
            private readonly Func<object> _resolveFactory;

            public PromiseNativeModule()
                : this(() => null)
            {
            }

            public PromiseNativeModule(Func<object> resolveFactory)
            {
                _resolveFactory = resolveFactory;
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(IPromise promise)
            {
                try
                {
                    promise.Resolve(_resolveFactory());
                }
                catch (Exception ex)
                {
                    promise.Reject(ex);
                }
            }
        }

        class PerfNativeModule : NativeModuleBase
        {
            public PerfNativeModule(IReactDelegateFactory delegateFactory)
                : base(delegateFactory)
            {
            }

            public override string Name
            {
                get
                {
                    return "Perf";
                }
            }

            [ReactMethod]
            public void Foo(int x) { }
        }
    }
}
