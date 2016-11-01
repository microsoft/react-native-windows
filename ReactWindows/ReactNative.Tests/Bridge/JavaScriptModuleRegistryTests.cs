using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.UIManager;
using ReactNative.UIManager.Events;
using System;
using System.Collections.Generic;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistryTests : JavaScriptModuleRegistrySharedTests
    {
        [Test]
        public void InvokesReactInstanceWhenFetchedModuleIsCalled()
        {
            _registry = _registryBuilder
                .Add(typeof(RCTEventEmitter))
                .Add(typeof(AppRegistry))
                .Add(typeof(TestJavaScriptModule))
                .Build();

            var are = new AutoResetEvent(false);
            var modules = new List<string>();
            var methods = new List<string>();
            var argsList = new List<JArray>();

            var reactInstance = new MockReactInstance((mod, met, args, tracingName) =>
            {
                modules.Add(mod);
                methods.Add(met);
                argsList.Add(args);
                are.Set();
            });

            var module = _registry.GetJavaScriptModule<TestJavaScriptModule>(reactInstance);

            module.Foo(42);

            are.WaitOne();

            Assert.AreEqual(1, modules.Count);
            Assert.AreEqual(1, methods.Count);
            Assert.AreEqual(1, modules.Count);

            Assert.AreEqual("TestJavaScriptModule", modules[0]);
            Assert.AreEqual("Foo", methods[0]);
            Assert.AreEqual(
                JArray.FromObject(new[] { 42 }).ToString(Formatting.None),
                argsList[0].ToString(Formatting.None));
        }

        [Test]
        public void ThrowsWhenUnknownModuleRequested()
        {
            _registry = _registryBuilder.Build();

            var reactInstance = new MockReactInstance();
            AssertEx.Throws<InvalidOperationException>(() => _registry.GetJavaScriptModule<TestJavaScriptModule>(reactInstance));
        }
    }
}