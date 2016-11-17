using NUnit.Framework;
using ReactNative.UIManager;
using System;
using System.Threading;
using System.Windows.Controls;

namespace ReactNative.Tests.UIManager
{
    [TestFixture, Apartment(ApartmentState.STA)]
    public class DependencyObjectExtensionsTests
    {
        [Test]
        public void FrameworkElementExtensions_ArgumentChecks()
        {
            ArgumentNullException ex1 = Assert.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetTag(null, 0));
            Assert.AreEqual("view", ex1.ParamName);

            ArgumentNullException ex2 = Assert.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetEventDispatcher(null, null));
            Assert.AreEqual("view", ex2.ParamName);

            ArgumentNullException ex3 = Assert.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetTag(null));
            Assert.AreEqual("view", ex3.ParamName);

            ArgumentNullException ex4 = Assert.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetEventDispatcher(null));
            Assert.AreEqual("view", ex4.ParamName);
        }

        [Test]
        public void FrameworkElementExtensions_Get_Set()
        {
            var button = new Button();

            button.SetTag(42);
            Assert.AreEqual(42, button.GetTag());

            button.SetEventDispatcher(null);
            Assert.IsNull(button.GetEventDispatcher());
        }

    }
}
