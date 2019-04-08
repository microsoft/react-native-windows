// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.Bridge;
using System;

namespace ReactNative.Tests.Bridge
{
    [TestFixture]
    public class ReactContextNativeModuleBaseTests
    {
        [Test]
        public void ReactContextNativeModuleBase_ArgumentChecks()
        {
            ArgumentNullException ex = Assert.Throws<ArgumentNullException>(
                () => { new TestModule(null); }
            );
            Assert.AreEqual("reactContext", ex.ParamName);

            var context = new ReactContext();
            var module = new TestModule(context);
            Assert.AreSame(context, module.Context);
        }

        class TestModule : ReactContextNativeModuleBase
        {
            public TestModule(ReactContext reactContext)
                : base(reactContext)
            {
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }
        }
    }
}
