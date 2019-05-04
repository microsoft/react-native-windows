// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using System.Collections.Generic;
using System.Threading;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class JavaScriptModuleRegistryTests
    {
        [Test]
        public void InvokesReactInstanceWhenFetchedModuleIsCalled()
        {
            var registry = new JavaScriptModuleRegistry();

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

            var module = registry.GetJavaScriptModule<TestJavaScriptModule>(reactInstance);

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
