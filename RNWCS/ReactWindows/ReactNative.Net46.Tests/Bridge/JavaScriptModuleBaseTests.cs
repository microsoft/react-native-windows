// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NMock;
using NUnit.Framework;
using ReactNative.Bridge;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleBaseTests
    {
        private MockFactory _mockFactory;
        private Mock<IInvocationHandler> _mockInvocationHandler;
        private TestJavaScriptModule _module;

        [SetUp]
        public void SetUp()
        {
            _mockFactory = new MockFactory();
            _mockInvocationHandler = _mockFactory.CreateMock<IInvocationHandler>();
            _module = new TestJavaScriptModule();
        }

        [TearDown]
        public void TearDown()
        {
            _mockFactory.VerifyAllExpectationsHaveBeenMet();
        }

        [Test]
        public void ThrowsWhenInvokeHandlerNotSet()
        {
            Assert.That(
                () => _module.Foo(),
                Throws.InvalidOperationException);
        }

        [Test]
        public void ThrowsWhenInvokeHandlerSetMoreThanOnce()
        {
            var otherMockInvocationHandler = _mockFactory.CreateMock<IInvocationHandler>();

            _module.InvocationHandler = _mockInvocationHandler.MockObject;
            Assert.That(
                () => _module.InvocationHandler = otherMockInvocationHandler.MockObject,
                Throws.InvalidOperationException);
        }

        [Test]
        public void InvokesSuppliedHandlerWithNoArguments()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            _mockInvocationHandler.Expects.One.Method(
                _ => _.Invoke(null, null))
                .With(nameof(TestJavaScriptModule.Foo), null);

            _module.Foo();

        }

        [Test]
        public void InvokesSuppliedHandlerWithOneArgument()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            _mockInvocationHandler.Expects.One.Method(
                _ => _.Invoke(null, null))
                .With(nameof(TestJavaScriptModule.Foo1), new object[] { 1 });

            _module.Foo1(1);
        }

        [Test]
        public void InvokesSuppliedHandlerWithTwoArguments()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            _mockInvocationHandler.Expects.One.Method
                (_ => _.Invoke(null, null))
                .With(nameof(TestJavaScriptModule.Foo2), new object[] { 1, 2 });

            _module.Foo2(1, 2);
        }

        [Test]
        public void InvokesSuppliedHandlerWithThreeArguments()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            _mockInvocationHandler.Expects.One.Method(
                _ => _.Invoke(null, null))
                .With(nameof(TestJavaScriptModule.Foo3), new object[] { 1, 2, 3 });

            _module.Foo3(1, 2, 3);
        }

        [Test]
        public void InvokesSuppliedHandlerWithFourArguments()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            _mockInvocationHandler.Expects.One.Method(
                _ => _.Invoke(null, null))
                .With(nameof(TestJavaScriptModule.Foo4), new object[] { 1, 2, 3, 4 });

            _module.Foo4(1, 2, 3, 4);
        }

        [Test]
        public void InvokesSuppliedHandlerWithArrayArgument()
        {
            _module.InvocationHandler = _mockInvocationHandler.MockObject;

            var expectedArgs = new object[] { null, "foo", 42 };
            _mockInvocationHandler.Expects.One.Method(
                _ => _.Invoke(null, null)).With(nameof(TestJavaScriptModule.FooN), expectedArgs);

            _module.FooN(expectedArgs);
        }

        class TestJavaScriptModule : JavaScriptModuleBase
        {
            public void Foo()
            {
                Invoke();
            }

            public void Foo1(object p0)
            {
                Invoke(p0);
            }

            public void Foo2(object p0, object p1)
            {
                Invoke(p0, p1);
            }

            public void Foo3(object p0, object p1, object p2)
            {
                Invoke(p0, p1, p2);
            }

            public void Foo4(object p0, object p1, object p2, object p3)
            {
                Invoke(p0, p1, p2, p3);
            }

            public void FooN(object[] ps)
            {
                Invoke(ps);
            }
        }
    }
}