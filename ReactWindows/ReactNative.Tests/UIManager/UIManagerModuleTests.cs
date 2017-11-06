using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Bridge;
using ReactNative.Bridge.Queue;
using ReactNative.Tests.Constants;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ReactNative.Tests.UIManager
{
    [TestClass]
    public class UIManagerModuleTests
    {
        [TestMethod]
        public void UIManagerModule_ArgumentChecks()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager>();
            var uiImplementationProvider = new UIImplementationProvider();

            using (var actionQueue = new ActionQueue(ex => { }))
            {
                AssertEx.Throws<ArgumentNullException>(
                    () => new UIManagerModule(context, null, uiImplementationProvider, actionQueue),
                    ex => Assert.AreEqual("viewManagers", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => new UIManagerModule(context, viewManagers, null, actionQueue),
                    ex => Assert.AreEqual("uiImplementationProvider", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => new UIManagerModule(context, viewManagers, uiImplementationProvider, null),
                    ex => Assert.AreEqual("layoutActionQueue", ex.ParamName));
            }
        }

        [TestMethod]
        public async Task UIManagerModule_CustomEvents_Constants()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager> { new NoEventsViewManager() };
            var uiImplementationProvider = new UIImplementationProvider();

            using (var actionQueue = new ActionQueue(ex => { }))
            {
                var module = await DispatcherHelpers.CallOnDispatcherAsync(
                    () => new UIManagerModule(context, viewManagers, uiImplementationProvider, actionQueue));

                var constants = module.Constants;

                Assert.AreEqual("onSelect", constants.GetMap("genericBubblingEventTypes").GetMap("topSelect").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onSelectCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topSelect").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onChange", constants.GetMap("genericBubblingEventTypes").GetMap("topChange").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onChangeCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topChange").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onTouchStart", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchStart").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onTouchStartCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchStart").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onTouchMove", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchMove").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onTouchMoveCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchMove").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onTouchEnd", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchEnd").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onTouchEndCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topTouchEnd").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onMouseOver", constants.GetMap("genericBubblingEventTypes").GetMap("topMouseOver").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onMouseOverCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topMouseOver").GetMap("phasedRegistrationNames").GetValue("captured"));
                Assert.AreEqual("onMouseOut", constants.GetMap("genericBubblingEventTypes").GetMap("topMouseOut").GetMap("phasedRegistrationNames").GetValue("bubbled"));
                Assert.AreEqual("onMouseOutCapture", constants.GetMap("genericBubblingEventTypes").GetMap("topMouseOut").GetMap("phasedRegistrationNames").GetValue("captured"));

                Assert.AreEqual("onSelectionChange", constants.GetMap("genericDirectEventTypes").GetMap("topSelectionChange").GetValue("registrationName"));
                Assert.AreEqual("onLoadingStart", constants.GetMap("genericDirectEventTypes").GetMap("topLoadingStart").GetValue("registrationName"));
                Assert.AreEqual("onLoadingFinish", constants.GetMap("genericDirectEventTypes").GetMap("topLoadingFinish").GetValue("registrationName"));
                Assert.AreEqual("onLoadingError", constants.GetMap("genericDirectEventTypes").GetMap("topLoadingError").GetValue("registrationName"));
                Assert.AreEqual("onLayout", constants.GetMap("genericDirectEventTypes").GetMap("topLayout").GetValue("registrationName"));
                Assert.AreEqual("onMouseEnter", constants.GetMap("genericDirectEventTypes").GetMap("topMouseEnter").GetValue("registrationName"));
                Assert.AreEqual("onMouseLeave", constants.GetMap("genericDirectEventTypes").GetMap("topMouseLeave").GetValue("registrationName"));
                Assert.AreEqual("onMessage", constants.GetMap("genericDirectEventTypes").GetMap("topMessage").GetValue("registrationName"));
            }
        }

        [TestMethod]
        public async Task UIManagerModule_Constants_ViewManagerOverrides()
        {
            var context = new ReactContext();
            var viewManagers = new List<IViewManager> { new TestViewManager() };
            var uiImplementationProvider = new UIImplementationProvider();

            using (var actionQueue = new ActionQueue(ex => { }))
            {
                var module = await DispatcherHelpers.CallOnDispatcherAsync(
                    () => new UIManagerModule(context, viewManagers, uiImplementationProvider, actionQueue));

                var constants = module.Constants.GetMap("Test");

                Assert.AreEqual(42, constants.GetMap("directEventTypes").GetValue("otherSelectionChange"));
                Assert.AreEqual(42, constants.GetMap("directEventTypes").GetMap("topSelectionChange").GetValue("registrationName"));
                Assert.AreEqual(42, constants.GetMap("directEventTypes").GetMap("topLoadingStart").GetValue("foo"));
                Assert.AreEqual(42, constants.GetMap("directEventTypes").GetValue("topLoadingError"));
            }
        }

        class NoEventsViewManager : MockViewManager
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
                    return new Dictionary<string, object>();
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

        class TestViewManager : NoEventsViewManager
        {
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
        }
    }
}
