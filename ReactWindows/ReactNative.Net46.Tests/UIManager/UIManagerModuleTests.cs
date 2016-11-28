using NUnit.Framework;
using ReactNative.Bridge;
using ReactNative.Tests.Constants;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace ReactNative.Tests.UIManager
{
    [TestFixture, Apartment(ApartmentState.STA)]
    public class UIManagerModuleTests
    {

        [Test]
        public void UIManagerModule_ArgumentChecks()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager>();
            var uiImplementationProvider = new UIImplementationProvider();

            ArgumentNullException ex1 = Assert.Throws<ArgumentNullException>(
                () => new UIManagerModule(context, null, uiImplementationProvider, new FrameworkElement()));
            Assert.AreEqual("viewManagers", ex1.ParamName);

            ArgumentNullException ex2 = Assert.Throws<ArgumentNullException>(
                () => new UIManagerModule(context, viewManagers, null, new FrameworkElement()));
            Assert.AreEqual("uiImplementationProvider", ex2.ParamName);
        }

        [Test]
        public async Task UIManagerModule_CustomEvents_Constants()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager>();
            var uiImplementationProvider = new UIImplementationProvider();

            var module = await DispatcherHelpers.CallOnDispatcherAsync(
                () => new UIManagerModule(context, viewManagers, uiImplementationProvider, new FrameworkElement()));

            var constants = module.Constants;

            Assert.AreEqual("onSelect", constants.GetMap("customBubblingEventTypes").GetMap("topSelect").GetMap("phasedRegistrationNames").GetValue("bubbled"));
            Assert.AreEqual("onSelectCapture", constants.GetMap("customBubblingEventTypes").GetMap("topSelect").GetMap("phasedRegistrationNames").GetValue("captured"));
            Assert.AreEqual("onChange", constants.GetMap("customBubblingEventTypes").GetMap("topChange").GetMap("phasedRegistrationNames").GetValue("bubbled"));
            Assert.AreEqual("onChangeCapture", constants.GetMap("customBubblingEventTypes").GetMap("topChange").GetMap("phasedRegistrationNames").GetValue("captured"));
            Assert.AreEqual("onTouchStart", constants.GetMap("customBubblingEventTypes").GetMap("topTouchStart").GetMap("phasedRegistrationNames").GetValue("bubbled"));
            Assert.AreEqual("onTouchStartCapture", constants.GetMap("customBubblingEventTypes").GetMap("topTouchStart").GetMap("phasedRegistrationNames").GetValue("captured"));
            Assert.AreEqual("onTouchMove", constants.GetMap("customBubblingEventTypes").GetMap("topTouchMove").GetMap("phasedRegistrationNames").GetValue("bubbled"));
            Assert.AreEqual("onTouchMoveCapture", constants.GetMap("customBubblingEventTypes").GetMap("topTouchMove").GetMap("phasedRegistrationNames").GetValue("captured"));
            Assert.AreEqual("onTouchEnd", constants.GetMap("customBubblingEventTypes").GetMap("topTouchEnd").GetMap("phasedRegistrationNames").GetValue("bubbled"));
            Assert.AreEqual("onTouchEndCapture", constants.GetMap("customBubblingEventTypes").GetMap("topTouchEnd").GetMap("phasedRegistrationNames").GetValue("captured"));

            Assert.AreEqual("onSelectionChange", constants.GetMap("customDirectEventTypes").GetMap("topSelectionChange").GetValue("registrationName"));
            Assert.AreEqual("onLoadingStart", constants.GetMap("customDirectEventTypes").GetMap("topLoadingStart").GetValue("registrationName"));
            Assert.AreEqual("onLoadingFinish", constants.GetMap("customDirectEventTypes").GetMap("topLoadingFinish").GetValue("registrationName"));
            Assert.AreEqual("onLoadingError", constants.GetMap("customDirectEventTypes").GetMap("topLoadingError").GetValue("registrationName"));
            Assert.AreEqual("onLayout", constants.GetMap("customDirectEventTypes").GetMap("topLayout").GetValue("registrationName"));
        }

        [Test]
        public async Task UIManagerModule_Constants_ViewManagerOverrides()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager> { new TestViewManager() };
            var uiImplementationProvider = new UIImplementationProvider();

            var module = await DispatcherHelpers.CallOnDispatcherAsync(
                () => new UIManagerModule(context, viewManagers, uiImplementationProvider, new FrameworkElement()));

            var constants = module.Constants;

            Assert.AreEqual(42, constants.GetMap("customDirectEventTypes").GetValue("otherSelectionChange"));
            Assert.AreEqual(42, constants.GetMap("customDirectEventTypes").GetMap("topSelectionChange").GetValue("registrationName"));
            Assert.AreEqual(42, constants.GetMap("customDirectEventTypes").GetMap("topLoadingStart").GetValue("foo"));
            Assert.AreEqual(42, constants.GetMap("customDirectEventTypes").GetValue("topLoadingError"));
        }

        class TestViewManager : MockViewManager
        {
            public override IReadOnlyDictionary<string, object> CommandsMap
            {
                get
                {
                    return null;
                }
            }

            public override IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
            {
                get
                {
                    return null;
                }
            }

            public override IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
            {
                get
                {
                    return new Dictionary<string, object>
                    {
                        { "otherSelectionChange", 42 },
                        { "topSelectionChange", new Dictionary<string, object> { { "registrationName", 42 } } },
                        { "topLoadingStart", new Dictionary<string, object> { { "foo", 42 } } },
                        { "topLoadingError", 42 },
                    };
                }
            }

            public override IReadOnlyDictionary<string, object> ExportedViewConstants
            {
                get
                {
                    return null;
                }
            }

            public override IReadOnlyDictionary<string, string> NativeProperties
            {
                get
                {
                    return null;
                }
            }

            public override string Name
            {
                get
                {
                    return "Test";
                }
            }

            public override Type ShadowNodeType
            {
                get
                {
                    return typeof(ReactShadowNode);
                }
            }
        }
    }
}
