using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.UIManager;
using System;
using Windows.UI.Xaml.Controls;

namespace ReactNative.Tests.UIManager
{
    [TestClass]
    public class DependencyObjectExtensionsTests
    {
        [Microsoft.VisualStudio.TestPlatform.UnitTestFramework.AppContainer.UITestMethod]
        public void FrameworkElementExtensions_ArgumentChecks()
        {
            var element = new Button();

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetTag(null, 0),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetEventDispatcher(null, null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetTag(null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetEventDispatcher(null),
                ex => Assert.AreEqual("view", ex.ParamName));
        }

        [Microsoft.VisualStudio.TestPlatform.UnitTestFramework.AppContainer.UITestMethod]
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
