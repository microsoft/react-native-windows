// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ViewManagerRegistryTests
    {
        [Test]
        public void ViewManagerRegistry_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => new ViewManagerRegistry(null),
                ex => Assert.AreEqual("viewManagers", ex.ParamName));

            var registry = new ViewManagerRegistry(new List<IViewManager>());

            AssertEx.Throws<ArgumentNullException>(
                () => registry.Get(null),
                ex => Assert.AreEqual("className", ex.ParamName));

            AssertEx.Throws<ArgumentException>(
                () => registry.Get("foo"),
                ex => Assert.AreEqual("className", ex.ParamName));
        }

        [Test]
        public void ViewManagerRegistry_Simple()
        {
            var viewManager = new TestViewManager();
            var registry = new ViewManagerRegistry(new List<IViewManager> { viewManager });
            Assert.AreSame(viewManager, registry.Get(viewManager.Name));
        }

        class TestViewManager : MockViewManager
        {
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
