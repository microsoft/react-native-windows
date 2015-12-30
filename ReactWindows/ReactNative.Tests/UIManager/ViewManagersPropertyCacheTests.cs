using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.UIManager;
using Windows.UI.Xaml;
using Newtonsoft.Json.Linq;

namespace ReactNative.Tests.UIManager
{
    [TestClass]
    public class ViewManagersPropertyCacheTests
    {
        [TestMethod]
        public void ViewManagersPropertyCache_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropertyCache.GetNativePropertiesForView(null, typeof(object)),
                ex => Assert.AreEqual("viewManagerType", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropertyCache.GetNativePropertiesForView(typeof(object), null),
                ex => Assert.AreEqual("shadowNodeType", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropertyCache.GetNativePropertySettersForViewManagerType(null),
                ex => Assert.AreEqual("type", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropertyCache.GetNativePropertySettersForShadowNodeType
                (null),
                ex => Assert.AreEqual("type", ex.ParamName));
        }

        [TestMethod]
        public void ViewManagersPropertyCache_ViewManager_Empty()
        {
            var setters = ViewManagersPropertyCache.GetNativePropertySettersForShadowNodeType(typeof(EmptyTest));
            Assert.AreEqual(0, setters.Count);
        }

        [TestMethod]
        public void ViewManagersPropertyCache_ShadowNode_Empty()
        {
            var setters = ViewManagersPropertyCache.GetNativePropertySettersForShadowNodeType(typeof(ReactShadowNode));
            Assert.AreEqual(0, setters.Count);
        }

        [TestMethod]
        public void ViewManagersPropertyCache_ViewManager_Set()
        {
            var instance = new ViewManagerValueTest();

            var setters = ViewManagersPropertyCache.GetNativePropertySettersForViewManagerType(typeof(ViewManagerValueTest));
            Assert.AreEqual(3, setters.Count);
            
            var properties = new CatalystStylesDiffMap(new JObject
            {
                { "Foo", "v1" },
                { "Bar1", "v2" },
                { "Bar2", "v3" },
            });

            AssertEx.Throws<NotSupportedException>(() => setters["Foo"].SetShadowNodeProperty(new ShadowNodeValueTest(), properties));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].SetViewManagerProperty(null, null, properties),
                ex => Assert.AreEqual("viewManager", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].SetViewManagerProperty(instance, null, null),
                ex => Assert.AreEqual("properties", ex.ParamName));

            setters["Foo"].SetViewManagerProperty(instance, null, properties);
            setters["Bar1"].SetViewManagerProperty(instance, null, properties);
            setters["Bar2"].SetViewManagerProperty(instance, null, properties);

            Assert.AreEqual("v1", instance.FooValue);
            Assert.AreEqual("v2", instance.BarValues[0]);
            Assert.AreEqual("v3", instance.BarValues[1]);
        }

        [TestMethod]
        public void ViewManagersPropertyCache_ShadowNode_Set()
        {
            var instance = new ShadowNodeValueTest();

            var setters = ViewManagersPropertyCache.GetNativePropertySettersForShadowNodeType(typeof(ShadowNodeValueTest));
            Assert.AreEqual(3, setters.Count);

            var properties = new CatalystStylesDiffMap(new JObject
            {
                { "Foo", "v1" },
                { "Bar1", "v2" },
                { "Bar2", "v3" },
            });

            AssertEx.Throws<NotSupportedException>(() => setters["Foo"].SetViewManagerProperty(new ViewManagerValueTest(), null, properties));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].SetShadowNodeProperty(null, properties),
                ex => Assert.AreEqual("shadowNode", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].SetShadowNodeProperty(instance, null),
                ex => Assert.AreEqual("properties", ex.ParamName));

            setters["Foo"].SetShadowNodeProperty(instance, properties);
            setters["Bar1"].SetShadowNodeProperty(instance, properties);
            setters["Bar2"].SetShadowNodeProperty(instance, properties);

            Assert.AreEqual("v1", instance.FooValue);
            Assert.AreEqual("v2", instance.BarValues[0]);
            Assert.AreEqual("v3", instance.BarValues[1]);
        }

        class EmptyTest : IViewManager
        {
            public IReadOnlyDictionary<string, object> CommandsMap
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedViewConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public string Name
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, string> NativeProperties
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public ReactShadowNode CreateShadowNodeInstance()
            {
                throw new NotImplementedException();
            }

            public void UpdateExtraData(FrameworkElement viewToUpdate, object extraData)
            {
                throw new NotImplementedException();
            }

            public void UpdateProperties(FrameworkElement viewToUpdate, CatalystStylesDiffMap properties)
            {
                throw new NotImplementedException();
            }
        }

        class ViewManagerValueTest : IViewManager
        {
            public string FooValue;

            [ReactProperty("Foo")]
            public void Foo(FrameworkElement element, string value)
            {
                FooValue = value;
            }

            public string[] BarValues = new string[2];

            [ReactPropertyGroup("Bar1", "Bar2")]
            public void Bar(FrameworkElement element, int index, string value)
            {
                BarValues[index] = value;
            }

            #region IViewManager Implementation

            public string Name
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> CommandsMap
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, object> ExportedViewConstants
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public IReadOnlyDictionary<string, string> NativeProperties
            {
                get
                {
                    throw new NotImplementedException();
                }
            }

            public ReactShadowNode CreateShadowNodeInstance()
            {
                throw new NotImplementedException();
            }

            public void UpdateProperties(FrameworkElement viewToUpdate, CatalystStylesDiffMap properties)
            {
                throw new NotImplementedException();
            }

            public void UpdateExtraData(FrameworkElement viewToUpdate, object extraData)
            {
                throw new NotImplementedException();
            }

            #endregion
        }

        class ShadowNodeValueTest : ReactShadowNode
        {
            public string FooValue;

            [ReactProperty("Foo")]
            public void Foo(string value)
            {
                FooValue = value;
            }

            public string[] BarValues = new string[2];

            [ReactPropertyGroup("Bar1", "Bar2")]
            public void Bar(int index, string value)
            {
                BarValues[index] = value;
            }
        }
    }
}
