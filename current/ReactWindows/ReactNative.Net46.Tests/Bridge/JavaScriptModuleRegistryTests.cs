// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NMock;
using NMock.Syntax;
using NUnit.Framework;
using ReactNative.Bridge;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistryTests
    {
        private MockFactory _mockFactory;
        private Mock<IReactInstance> _mockReactInstance;

        [SetUp]
        public void SetUp()
        {
            _mockFactory = new MockFactory();
            _mockReactInstance = _mockFactory.CreateMock<IReactInstance>();
        }

        [Test]
        public void InvokesReactInstanceWhenFetchedModuleIsCalled()
        {
            var registry = new JavaScriptModuleRegistry();

            var are = new AutoResetEvent(false);

            var module = registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance.MockObject);

            _mockReactInstance.Expects.One.Method(
                _ => _.InvokeFunction(null, null, null, null))
                .With(
                    nameof(TestJavaScriptModule),
                    nameof(TestJavaScriptModule.Foo),
                    JArray.FromObject(new[] { 42 }),
                    NMock.Is.StringContaining("_"))
                .Will(Invoke.Action(() => are.Set()));

            module.Foo(42);

            are.WaitOne();
        }

        class TestJavaScriptModule : JavaScriptModuleBase
        {
            public void Bar()
            {
                Invoke();
            }

            public void Baz()
            {
                Invoke();
            }

            public void Foo(int x)
            {
                Invoke(x);
            }
        }
    }
}
