// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json.Linq;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  [TestClass]
  public class JSValueTest
  {
    [TestMethod]
    public void TestReadObject()
    {
      JObject jobj = JObject.Parse(@"{
              NullValue: null,
              ObjValue: {},
              ArrayValue: [],
              StringValue: ""Hello"",
              BoolValue: true,
              IntValue: 42,
              DoubleValue: 4.5
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);

      JSValue jsValue = JSValue.ReadFrom(reader);
      Assert.AreEqual(JSValueType.Object, jsValue.Type);
      Assert.IsTrue(jsValue.Object["NullValue"].IsNull);
      Assert.IsNotNull(jsValue.Object["ObjValue"].Object);
      Assert.IsNotNull(jsValue.Object["ArrayValue"].Array);
      Assert.AreEqual("Hello", jsValue.Object["StringValue"].String);
      Assert.AreEqual(true, jsValue.Object["BoolValue"].Boolean);
      Assert.AreEqual(42, jsValue.Object["IntValue"].Int64);
      Assert.AreEqual(4.5, jsValue.Object["DoubleValue"].Double);
    }

    [TestMethod]
    public void TestReadNestedObject()
    {
      JObject jobj = JObject.Parse(@"{
              NestedObj: {
                NullValue: null,
                ObjValue: {},
                ArrayValue: [],
                StringValue: ""Hello"",
                BoolValue: true,
                IntValue: 42,
                DoubleValue: 4.5
              }
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);

      JSValue jsValue = JSValue.ReadFrom(reader);
      Assert.AreEqual(JSValueType.Object, jsValue.Type);
      var nestedObj = jsValue.Object["NestedObj"].Object;
      Assert.IsTrue(nestedObj["NullValue"].IsNull);
      Assert.IsNotNull(nestedObj["ObjValue"].Object);
      Assert.IsNotNull(nestedObj["ArrayValue"].Array);
      Assert.AreEqual("Hello", nestedObj["StringValue"].String);
      Assert.AreEqual(true, nestedObj["BoolValue"].Boolean);
      Assert.AreEqual(42, nestedObj["IntValue"].Int64);
      Assert.AreEqual(4.5, nestedObj["DoubleValue"].Double);
    }

    [TestMethod]
    public void TestReadArray()
    {
      JArray jarr = JArray.Parse(@"[null, {}, [], ""Hello"", true, 42, 4.5]");
      IJSValueReader reader = new JTokenJSValueReader(jarr);

      JSValue jsValue = JSValue.ReadFrom(reader);
      Assert.AreEqual(JSValueType.Array, jsValue.Type);
      Assert.IsTrue(jsValue.Array[0].IsNull);
      Assert.IsNotNull(jsValue.Array[1].Object);
      Assert.IsNotNull(jsValue.Array[2].Array);
      Assert.AreEqual("Hello", jsValue.Array[3].String);
      Assert.AreEqual(true, jsValue.Array[4].Boolean);
      Assert.AreEqual(42, jsValue.Array[5].Int64);
      Assert.AreEqual(4.5, jsValue.Array[6].Double);
    }

    [TestMethod]
    public void TestReadNestedArray()
    {
      JArray jarr = JArray.Parse(@"[[null, {}, [], ""Hello"", true, 42, 4.5]]");
      IJSValueReader reader = new JTokenJSValueReader(jarr);

      JSValue jsValue = JSValue.ReadFrom(reader);
      Assert.AreEqual(JSValueType.Array, jsValue.Type);
      var nestedArr = jsValue.Array[0].Array;
      Assert.IsTrue(nestedArr[0].IsNull);
      Assert.IsNotNull(nestedArr[1].Object);
      Assert.IsNotNull(nestedArr[2].Array);
      Assert.AreEqual("Hello", nestedArr[3].String);
      Assert.AreEqual(true, nestedArr[4].Boolean);
      Assert.AreEqual(42, nestedArr[5].Int64);
      Assert.AreEqual(4.5, nestedArr[6].Double);
    }
  }
}
