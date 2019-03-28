// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class NativeModuleWrapperBaseTests
    {
        [Test]
        public void NativeModuleWrapperBase_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(() => new TestWrapper(null), ex => Assert.AreEqual("module", ex.ParamName));

            var wrapper = new TestWrapper(new TestModule());
            AssertEx.Throws<ArgumentNullException>(() => wrapper.TestNullAction(), ex => Assert.AreEqual("action", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(() => wrapper.TestNullCallback(), ex => Assert.AreEqual("invokeCallback", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(() => wrapper.TestNullFunc(), ex => Assert.AreEqual("func", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(() => wrapper.TestNullTypeAction(), ex => Assert.AreEqual("type", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(() => wrapper.TestNullTypeFunc(), ex => Assert.AreEqual("type", ex.ParamName));
        }

        [Test]
        public async Task NativeModuleWrapperBase_ProxiedValues()
        {
            var initialized = 0;
            var disposed = 0;
            var module = new TestModule
            {
                TestConstants = new JObject(),
                OnInitialize = () => initialized++,
                OnOnReactInstanceDispose = () => disposed++,
            };

            var wrapper = new TestWrapper(module);

            Assert.AreEqual(module.Name, wrapper.Name);
            Assert.AreEqual(true, wrapper.CanOverrideExistingModule);
            Assert.AreSame(module.ModuleConstants, wrapper.Constants);

            wrapper.Initialize();
            Assert.AreEqual(1, initialized);
            await wrapper.DisposeAsync();
            Assert.AreEqual(1, disposed);
        }

        class TestWrapper : NativeModuleWrapperBase<TestModule>
        {
            public TestWrapper(TestModule module) : base(module) { }

            public override IReadOnlyDictionary<string, INativeMethod> Methods => new Dictionary<string, INativeMethod>();

            public TValue CastJTokenTest<TValue>(JToken value) where TValue : JToken
            {
                return CastJToken<TValue>(value);
            }

            public void TestNullTypeAction()
            {
                var method = new NativeMethod(null, (_, __) => { });
            }

            public void TestNullTypeFunc()
            {
                var method = new NativeMethod(null, (_, __) => default(JToken));
            }

            public void TestNullAction()
            {
                var method = new NativeMethod("foo", default(Action<InvokeCallback, JArray>));
            }

            public void TestNullFunc()
            {
                var method = new NativeMethod("foo", default(Func<InvokeCallback, JArray, JToken>));
            }

            public void TestNullCallback()
            {
                var callback = new Callback(0, null);
            }
        }

        class TestModule : NativeModuleBase
        {
            public override string Name => "Test";

            public override bool CanOverrideExistingModule => true;

            public JObject TestConstants;

            public override JObject ModuleConstants => TestConstants;

            public Action OnInitialize;

            public override void Initialize()
            {
                OnInitialize?.Invoke();
                base.Initialize();
            }

            public Action OnOnReactInstanceDispose;

            public override Task OnReactInstanceDisposeAsync()
            {
                OnOnReactInstanceDispose?.Invoke();
                return base.OnReactInstanceDisposeAsync();
            }
        }
    }
}
