using NMock;
using NUnit.Framework;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Windows;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ViewManagerRegistryTests
    {
        [Test]
        public void ViewManagerRegistry_ArgumentChecks()
        {
            ArgumentNullException ex1 = Assert.Throws<ArgumentNullException>(
                () => new ViewManagerRegistry(null));
            Assert.AreEqual("viewManagers", ex1.ParamName);

            var registry = new ViewManagerRegistry(new List<IViewManager>());

            ArgumentNullException ex2 = Assert.Throws<ArgumentNullException>(
                () => registry.Get(null));
            Assert.AreEqual("className", ex2.ParamName);

            ArgumentException ex3 = Assert.Throws<ArgumentException>(
                () => registry.Get("foo"));
            Assert.AreEqual("className", ex3.ParamName);
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
