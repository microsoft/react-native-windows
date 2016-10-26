using NUnit.Framework;
using ReactNative.UIManager;
using System.Windows;
using System.Windows.Controls;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class RootViewHelperTests
    {
        [Test]
        public void RootViewHelper_Null()
        {
            Assert.IsNull(RootViewHelper.GetRootView(null));
        }

        class TestRootView : Panel, IRootView
        {
            public void OnChildStartedNativeGesture(RoutedEventArgs ev)
            {
            }
        }
    }
}
