using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistryTests : JavaScriptModuleRegistrySharedTests
    {
        private MockReactInstance _mockReactInstance;

        [SetUp]
        public override void SetUp()
        {
            base.SetUp();

            _mockReactInstance = new MockReactInstance();
        }

        [Test]
        public void InvokesReactInstanceWhenFetchedModuleIsCalled()
        {
            _registry = _registryBuilder
                .Add(typeof(RCTEventEmitter))
                .Add(typeof(AppRegistry))
                .Add(typeof(TestJavaScriptModule))
                .Build();

            var are = new AutoResetEvent(false);

            var module = _registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance);

            _mockReactInstance.InvokeFunction(nameof(TestJavaScriptModule), nameof(TestJavaScriptModule.Foo),
                JArray.FromObject(new[] {42}), "tracingName");

            //_mockReactInstance.Expects.One.Method(
            //    _ => _.InvokeFunction(null, null, null, null))
            //    .With(
            //        nameof(TestJavaScriptModule),
            //        nameof(TestJavaScriptModule.Foo),
            //        JArray.FromObject(new[] { 42 }),
            //        NMock.Is.StringContaining("_"))
            //    .Will(Invoke.Action(() => are.Set()));

            module.Foo(42);

            are.WaitOne();
        }

        [Test]
        public void ThrowsWhenUnknownModuleRequested()
        {
            _registry = _registryBuilder.Build();

            Assert.Throws<InvalidOperationException>(
                () => { _registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance); }
            );

            //Assert.That(
            //    () => _registry.GetJavaScriptModule<TestJavaScriptModule>(_mockReactInstance.MockObject),
            //    Throws.InvalidOperationException);
        }
    }
}