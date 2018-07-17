// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.Bridge;
using System;
using System.IO;
using System.Linq;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class NativeModuleRegistryTests
    {
        [Test]
        public void NativeModuleRegistry_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => new NativeModuleRegistry.Builder(null),
                ex => Assert.AreEqual("reactContext", ex.ParamName));

            var builder = new NativeModuleRegistry.Builder(new ReactContext());
            AssertEx.Throws<ArgumentNullException>(
                () => builder.Add(null),
                ex => Assert.AreEqual("module", ex.ParamName));
        }

        [Test]
        public void NativeModuleRegistry_Override_Disallowed()
        {
            var builder = new NativeModuleRegistry.Builder(new ReactContext());
            builder.Add(new OverrideDisallowedModule());
            AssertEx.Throws<InvalidOperationException>(() => builder.Add(new OverrideDisallowedModule()));
        }

        [Test]
        public void NativeModuleRegistry_Override_Allowed()
        {
            var registry = new NativeModuleRegistry.Builder(new ReactContext())
                .Add(new OverrideAllowedModule())
                .Add(new OverrideAllowedModule())
                .Build();

            Assert.AreEqual(1, registry.Modules.Count());
        }

        [Test]
        public void NativeModuleRegistry_ModuleWithNullName_Throws()
        {
            var builder = new NativeModuleRegistry.Builder(new ReactContext());
            AssertEx.Throws<ArgumentException>(
                () => builder.Add(new NullNameModule()),
                ex => Assert.AreEqual("module", ex.ParamName));
        }

        [Test]
        public void NativeModuleRegistry_WriteModuleDefinitions()
        {
            var registry = new NativeModuleRegistry.Builder(new ReactContext())
                .Add(new TestNativeModule())
                .Build();

            using (var stringWriter = new StringWriter())
            {
                using (var writer = new JsonTextWriter(stringWriter))
                {
                    registry.WriteModuleDescriptions(writer);
                }

                var json = JArray.Parse(stringWriter.ToString());
                Assert.AreEqual(1, json.Count);

                var module = json[0] as JArray;
                Assert.IsNotNull(module);
                Assert.IsTrue(module.Count >= 3);

                var moduleName = module[0];
                Assert.AreEqual("Test", moduleName.ToString());

                var methods = json[0][2] as JArray;
                Assert.IsNotNull(methods);

                var methodNames = methods.ToObject<string[]>();
                Assert.IsTrue(methodNames.Contains("Foo"));
                Assert.IsTrue(methodNames.Contains("Bar"));
            }
        }

        class OverrideDisallowedModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Foo";
                }
            }
        }

        class OverrideAllowedModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Foo";
                }
            }

            public override bool CanOverrideExistingModule
            {
                get
                {
                    return true;
                }
            }
        }

        class NullNameModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return null;
                }
            }
        }

        class TestNativeModule : NativeModuleBase
        {
            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            [ReactMethod]
            public void Foo(int x) { }

            [ReactMethod]
            public void Bar(string x) { }
        }
    }
}
