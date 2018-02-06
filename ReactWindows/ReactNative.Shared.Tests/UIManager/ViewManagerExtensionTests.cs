using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
#else
using System.Windows;
using System.Windows.Controls;
#endif

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    class ViewManagerExtensionTests
    {
        [Test]
        public void ViewManagerExtension_ArgumentChecks()
        {
            var vmx = new TestViewManagerExtension();
            AssertEx.Throws<ArgumentNullException>(
                () => vmx.UpdateProperties(null, null),
                ex => Assert.AreEqual("props", ex.ParamName));
        }

        [Test]
        public void ViewManagerExtension_AddEventEmitters()
        {
            var vmx = new TestViewManagerExtension();
            var count = 0;
            vmx.AddEventEmittersAction = () => count++;
            vmx.OnCreateView(null, null);
            Assert.AreEqual(1, count);
        }

        [Test]
        public void ViewManagerExtension_NativeProperties()
        {
            var vmx = new TestViewManagerExtension();
            var props = vmx.NativeProperties;
            Assert.IsTrue(props.ContainsKey("foo"));
            Assert.IsTrue(props.ContainsKey("bar"));
            Assert.IsTrue(props.ContainsKey("qux"));
            Assert.AreEqual("async", props["foo"]);
            Assert.AreEqual("async", props["bar"]);
            Assert.AreEqual("async", props["qux"]);
        }

        [Test]
        public void ViewManagerExtension_UpdateProperties()
        {
            var vmx = new TestViewManagerExtension();
            var foo = 42;
            var diffMap = new ReactStylesDiffMap(new JObject
            {
                { "foo", foo },
            });

            var count = 0;
            vmx.SetFooAction = x =>
            {
                Assert.AreEqual(foo, x);
                count++;
            };

            vmx.UpdateProperties(null, diffMap);
            Assert.AreEqual(1, count);
        }

        [Test]
        public void ViewManagerExtension_UpdateProperties_Group()
        {
            var vmx = new TestViewManagerExtension();
            var v1 = "hello";
            var v2 = "world";
            var diffMap = new ReactStylesDiffMap(new JObject
            {
                { "bar", v1 },
                { "qux", v2 },
            });

            var count = 0;
            vmx.SetOtherAction = (index, x) =>
            {
                Assert.IsTrue(index == 0 || index == 1);
                Assert.AreEqual(index == 0 ? v1 : v2, x);
                count++;
            };

            vmx.UpdateProperties(null, diffMap);
            Assert.AreEqual(2, count);
        }

        [Test]
        public void ViewManagerExtension_OnAfterUpdateTransaction()
        {
            var vmx = new TestViewManagerExtension();
            var count = 0;
            vmx.OnAfterUpdateTransactionAction = () => count++;
            vmx.UpdateProperties(null, new ReactStylesDiffMap(new JObject()));
            Assert.AreEqual(1, count);
        }

        #region Test Classes

        class DummyViewManager : IViewManager
        {
            public string Name => throw new NotImplementedException();

            public Type ShadowNodeType => throw new NotImplementedException();

            public IReadOnlyDictionary<string, object> CommandsMap => throw new NotImplementedException();

            public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants => throw new NotImplementedException();

            public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants => throw new NotImplementedException();

            public IReadOnlyDictionary<string, object> ExportedViewConstants => throw new NotImplementedException();

            public IReadOnlyDictionary<string, string> NativeProperties => throw new NotImplementedException();

            public ReactShadowNode CreateShadowNodeInstance()
            {
                throw new NotImplementedException();
            }

            public DependencyObject CreateView(ThemedReactContext reactContext)
            {
                throw new NotImplementedException();
            }

            public Dimensions GetDimensions(DependencyObject view)
            {
                throw new NotImplementedException();
            }

            public void OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view)
            {
                throw new NotImplementedException();
            }

            public void ReceiveCommand(DependencyObject view, int commandId, JArray args)
            {
                throw new NotImplementedException();
            }

            public void SetDimensions(DependencyObject view, Dimensions dimensions)
            {
                throw new NotImplementedException();
            }

            public void UpdateExtraData(DependencyObject root, object extraData)
            {
                throw new NotImplementedException();
            }

            public void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props)
            {
                throw new NotImplementedException();
            }
        }

        class TestViewManagerExtension : ViewManagerExtension<Canvas, DummyViewManager>
        {
            public Action AddEventEmittersAction { get; set; }

            protected override void AddEventEmitters(ThemedReactContext reactContext, Canvas view) => AddEventEmittersAction?.Invoke();

            public Action OnAfterUpdateTransactionAction { get; set; }

            protected override void OnAfterUpdateTransaction(Canvas view) => OnAfterUpdateTransactionAction?.Invoke();

            public Action<int> SetFooAction { get; set; }

            [ReactProp("foo")]
            public void SetFoo(Canvas view, int foo) => SetFooAction?.Invoke(foo);

            public Action<int, string> SetOtherAction { get; set; }

            [ReactPropGroup("bar", "qux")]
            public void SetOther(Canvas view, int id, string value) => SetOtherAction?.Invoke(id, value);
        }

        #endregion
    }
}
