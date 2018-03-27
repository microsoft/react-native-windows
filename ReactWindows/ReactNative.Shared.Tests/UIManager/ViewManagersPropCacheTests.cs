// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using System;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ViewManagersPropCacheTests
    {
        [Test]
        public void ViewManagersPropCache_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropCache.GetNativePropsForView<object>(null, typeof(object)),
                ex => Assert.AreEqual("viewManagerType", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropCache.GetNativePropsForView<object>(typeof(object), null),
                ex => Assert.AreEqual("shadowNodeType", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropCache.GetNativePropSettersForViewManagerType<object>(null),
                ex => Assert.AreEqual("type", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewManagersPropCache.GetNativePropSettersForShadowNodeType
                (null),
                ex => Assert.AreEqual("type", ex.ParamName));
        }

        [Test]
        public void ViewManagersPropCache_ViewManager_Empty()
        {
            var setters = ViewManagersPropCache.GetNativePropSettersForShadowNodeType(typeof(EmptyTest));
            Assert.AreEqual(0, setters.Count);
        }

        [Test]
        public void ViewManagersPropCache_ShadowNode_Empty()
        {
            var setters = ViewManagersPropCache.GetNativePropSettersForShadowNodeType(typeof(ReactShadowNode));
            Assert.AreEqual(0, setters.Count);
        }

        [Test]
        public void ViewManagersPropCache_ViewManager_Set()
        {
            var instance = new ViewManagerValueTest();

            var setters = ViewManagersPropCache.GetNativePropSettersForViewManagerType<object>(typeof(ViewManagerValueTest));
            Assert.AreEqual(3, setters.Count);

            var props = new JObject
            {
                { "Foo", "v1" },
                { "Bar1", "v2" },
                { "Bar2", "v3" },
            };

            AssertEx.Throws<NotSupportedException>(() => setters["Foo"].UpdateShadowNodeProp(new ShadowNodeValueTest(), props));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].UpdateViewManagerProp(null, null, props),
                ex => Assert.AreEqual("viewManager", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].UpdateViewManagerProp(instance, null, null),
                ex => Assert.AreEqual("props", ex.ParamName));

            setters["Foo"].UpdateViewManagerProp(instance, null, props);
            setters["Bar1"].UpdateViewManagerProp(instance, null, props);
            setters["Bar2"].UpdateViewManagerProp(instance, null, props);

            Assert.AreEqual("v1", instance.FooValue);
            Assert.AreEqual("v2", instance.BarValues[0]);
            Assert.AreEqual("v3", instance.BarValues[1]);
        }

        [Test]
        public void ViewManagersPropCache_ShadowNode_Set()
        {
            var instance = new ShadowNodeValueTest();

            var setters = ViewManagersPropCache.GetNativePropSettersForShadowNodeType(typeof(ShadowNodeValueTest));
            Assert.AreEqual(3, setters.Count);

            var props = new JObject
            {
                { "Foo", 42 },
                { "Qux1", "v2" },
                { "Qux2", "v3" },
            };

            AssertEx.Throws<NotSupportedException>(() => setters["Foo"].UpdateViewManagerProp(new ViewManagerValueTest(), null, props));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].UpdateShadowNodeProp(null, props),
                ex => Assert.AreEqual("shadowNode", ex.ParamName));
            AssertEx.Throws<ArgumentNullException>(
                () => setters["Foo"].UpdateShadowNodeProp(instance, null),
                ex => Assert.AreEqual("props", ex.ParamName));

            setters["Foo"].UpdateShadowNodeProp(instance, props);
            setters["Qux1"].UpdateShadowNodeProp(instance, props);
            setters["Qux2"].UpdateShadowNodeProp(instance, props);

            Assert.AreEqual(42, instance.FooValue);
            Assert.AreEqual("v2", instance.QuxValues[0]);
            Assert.AreEqual("v3", instance.QuxValues[1]);
        }

        [Test]
        public void ViewManagersPropCache_GetNativePropsForView()
        {
            var props = ViewManagersPropCache.GetNativePropsForView<object>(typeof(ViewManagerValueTest), typeof(ShadowNodeValueTest));
            Assert.AreEqual(5, props.Count);
            Assert.AreEqual("number", props["Foo"].Value<string>());
            Assert.AreEqual("String", props["Bar1"].Value<string>());
            Assert.AreEqual("String", props["Bar2"].Value<string>());
            Assert.AreEqual("String", props["Qux1"].Value<string>());
            Assert.AreEqual("String", props["Qux2"].Value<string>());
        }

        [Test]
        public void ViewManagersPropCache_Defaults()
        {
            var instance = new DefaultsTest();
            var setters = ViewManagersPropCache.GetNativePropSettersForViewManagerType<object>(typeof(DefaultsTest));

            var props = new JObject();

            instance.ByteValue = byte.MaxValue;
            instance.SByteValue = sbyte.MaxValue;
            instance.Int16Value = short.MaxValue;
            instance.UInt16Value = ushort.MaxValue;
            instance.Int32Value = int.MaxValue;
            instance.UInt32Value = uint.MaxValue;
            instance.Int64Value = long.MaxValue;
            instance.UInt64Value = ulong.MaxValue;
            instance.SingleValue = float.MaxValue;
            instance.DoubleValue = double.MaxValue;
            instance.DecimalValue = decimal.MaxValue;
            instance.BooleanValue = true;
            instance.StringValue = "foo";
            instance.ArrayValue = new int[0];
            instance.MapValue = new object();
            instance.NullableValue = true;
            instance.GroupValue = new[] { "a", "b", "c" };

            setters["TestByte"].UpdateViewManagerProp(instance, null, props);
            setters["TestSByte"].UpdateViewManagerProp(instance, null, props);
            setters["TestInt16"].UpdateViewManagerProp(instance, null, props);
            setters["TestUInt16"].UpdateViewManagerProp(instance, null, props);
            setters["TestInt32"].UpdateViewManagerProp(instance, null, props);
            setters["TestUInt32"].UpdateViewManagerProp(instance, null, props);
            setters["TestInt64"].UpdateViewManagerProp(instance, null, props);
            setters["TestUInt64"].UpdateViewManagerProp(instance, null, props);
            setters["TestSingle"].UpdateViewManagerProp(instance, null, props);
            setters["TestDouble"].UpdateViewManagerProp(instance, null, props);
            setters["TestDecimal"].UpdateViewManagerProp(instance, null, props);
            setters["TestBoolean"].UpdateViewManagerProp(instance, null, props);
            setters["TestString"].UpdateViewManagerProp(instance, null, props);
            setters["TestArray"].UpdateViewManagerProp(instance, null, props);
            setters["TestMap"].UpdateViewManagerProp(instance, null, props);
            setters["TestNullable"].UpdateViewManagerProp(instance, null, props);
            setters["foo"].UpdateViewManagerProp(instance, null, props);
            setters["bar"].UpdateViewManagerProp(instance, null, props);
            setters["baz"].UpdateViewManagerProp(instance, null, props);

            Assert.AreEqual(0, instance.ByteValue);
            Assert.AreEqual(0, instance.SByteValue);
            Assert.AreEqual(0, instance.Int16Value);
            Assert.AreEqual(0, instance.UInt16Value);
            Assert.AreEqual(0, instance.Int32Value);
            Assert.AreEqual((uint)0, instance.UInt32Value);
            Assert.AreEqual(0, instance.Int64Value);
            Assert.AreEqual((ulong)0, instance.UInt64Value);
            Assert.AreEqual(0, instance.SingleValue);
            Assert.AreEqual(0, instance.DoubleValue);
            Assert.AreEqual(0, instance.DecimalValue);
            Assert.IsFalse(instance.BooleanValue);
            Assert.IsNull(instance.StringValue);
            Assert.IsNull(instance.ArrayValue);
            Assert.IsNull(instance.MapValue);
            Assert.IsFalse(instance.NullableValue.HasValue);
            Assert.IsNull(instance.GroupValue[0]);
            Assert.IsNull(instance.GroupValue[1]);
            Assert.IsNull(instance.GroupValue[2]);
        }

        class EmptyTest : MockViewManager
        {
        }

        class ViewManagerValueTest : MockViewManager
        {
            public string FooValue;

            [ReactProp("Foo")]
            public void Foo(object element, string value)
            {
                FooValue = value;
            }

            public string[] BarValues = new string[2];

            [ReactPropGroup("Bar1", "Bar2")]
            public void Bar(object element, int index, string value)
            {
                BarValues[index] = value;
            }
        }

        class ShadowNodeValueTest : ReactShadowNode
        {
            public int FooValue;

            [ReactProp("Foo")]
            public void Foo(int value)
            {
                FooValue = value;
            }

            public string[] QuxValues = new string[2];

            [ReactPropGroup("Qux1", "Qux2")]
            public void Qux(int index, string value)
            {
                QuxValues[index] = value;
            }
        }

        class DefaultsTest : MockViewManager
        {
#region ViewManager Test Methods

            public byte ByteValue;
            public sbyte SByteValue;
            public short Int16Value;
            public ushort UInt16Value;
            public int Int32Value;
            public uint UInt32Value;
            public long Int64Value;
            public ulong UInt64Value;
            public float SingleValue;
            public double DoubleValue;
            public decimal DecimalValue;
            public bool BooleanValue;
            public string StringValue;
            public int[] ArrayValue;
            public object MapValue;
            public bool? NullableValue;
            public string[] GroupValue = new string[3];

            [ReactProp("TestByte")]
            public void TestByte(object element, byte value)
            {
                ByteValue = value;
            }

            [ReactProp("TestSByte")]
            public void TestSByte(object element, sbyte value)
            {
                SByteValue = value;
            }

            [ReactProp("TestInt16")]
            public void TestInt16(object element, short value)
            {
                Int16Value = value;
            }

            [ReactProp("TestUInt16")]
            public void TestUInt16(object element, ushort value)
            {
                UInt16Value = value;
            }

            [ReactProp("TestInt32")]
            public void TestInt32(object element, int value)
            {
                Int32Value = value;
            }

            [ReactProp("TestUInt32")]
            public void TestUInt32(object element, uint value)
            {
                UInt32Value = value;
            }

            [ReactProp("TestInt64")]
            public void TestInt64(object element, long value)
            {
                Int64Value = value;
            }

            [ReactProp("TestUInt64")]
            public void TestUInt64(object element, ulong value)
            {
                UInt64Value = value;
            }

            [ReactProp("TestSingle")]
            public void TestSingle(object element, float value)
            {
                SingleValue = value;
            }

            [ReactProp("TestDouble")]
            public void TestDouble(object element, double value)
            {
                DoubleValue = value;
            }

            [ReactProp("TestDecimal")]
            public void TestDecimal(object element, decimal value)
            {
                DecimalValue = value;
            }

            [ReactProp("TestBoolean")]
            public void TestBoolean(object element, bool value)
            {
                BooleanValue = value;
            }

            [ReactProp("TestString")]
            public void TestString(object element, string value)
            {
                StringValue = value;
            }

            [ReactProp("TestArray")]
            public void TestArray(object element, int[] value)
            {
                ArrayValue = value;
            }

            [ReactProp("TestNullable")]
            public void TestNullable(object element, bool? value)
            {
                NullableValue = value;
            }

            [ReactProp("TestMap")]
            public void TestMap(object element, object value)
            {
                MapValue = value;
            }

            [ReactPropGroup("foo", "bar", "baz")]
            public void TestGroup(object element, int index, string value)
            {
                GroupValue[index] = value;
            }

#endregion
        }
    }
}
