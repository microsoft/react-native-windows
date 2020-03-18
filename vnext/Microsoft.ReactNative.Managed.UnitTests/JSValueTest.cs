// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  [TestClass]
  public class JSValueTest
  {
    [TestMethod]
    public void TestReadObject()
    {
      JObject jobj = JObject.Parse(@"{
              ""NullValue"": null,
              ""ObjValue"": {""prop1"": 2},
              ""ArrayValue"": [1, 2],
              ""StringValue"": ""Hello"",
              ""BoolValue"": true,
              ""IntValue"": 42,
              ""DoubleValue"": 4.5
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);
      JSValue jsValue = JSValue.ReadFrom(reader);

      Assert.AreEqual(JSValueType.Object, jsValue.Type, "tag_a101");
      Assert.AreEqual(JSValueType.Null, jsValue["NullValue"].Type, "tag_1a02");
      Assert.AreEqual(JSValueType.Object, jsValue["ObjValue"].Type, "tag_a103");
      Assert.AreEqual(JSValueType.Array, jsValue["ArrayValue"].Type, "tag_a104");
      Assert.AreEqual(JSValueType.String, jsValue["StringValue"].Type, "tag_a105");
      Assert.AreEqual(JSValueType.Boolean, jsValue["BoolValue"].Type, "tag_a106");
      Assert.AreEqual(JSValueType.Int64, jsValue["IntValue"].Type, "tag_a107");
      Assert.AreEqual(JSValueType.Double, jsValue["DoubleValue"].Type, "tag_a108");

      Assert.IsTrue(jsValue["NullValue"].IsNull, "tag_a201");
      Assert.IsTrue(jsValue["ObjValue"].TryGetObject(out var objValue), "tag_a202");
      Assert.IsTrue(jsValue["ArrayValue"].TryGetArray(out var arrayValue), "tag_a203");
      Assert.IsTrue(jsValue["StringValue"].TryGetString(out var stringValue), "tag_a204");
      Assert.IsTrue(jsValue["BoolValue"].TryGetBoolean(out var boolValue), "tag_a205");
      Assert.IsTrue(jsValue["IntValue"].TryGetInt64(out var intValue), "tag_a206");
      Assert.IsTrue(jsValue["DoubleValue"].TryGetDouble(out var doubleValue), "tag_a207");

      Assert.AreEqual(1, objValue.Count, "tag_a301");
      Assert.AreEqual(1, jsValue["ObjValue"].PropertyCount, "tag_a302");
      Assert.AreEqual(2, arrayValue.Count, "tag_a303");
      Assert.AreEqual(2, jsValue["ArrayValue"].ItemCount, "tag_a304");
      Assert.AreEqual("Hello", stringValue, "tag_a305");
      Assert.AreEqual(true, boolValue, "tag_a306");
      Assert.AreEqual(42, intValue, "tag_a307");
      Assert.AreEqual(4.5, doubleValue, "tag_a308");
    }

    [TestMethod]
    public void TestReadNestedObject()
    {
      JObject jobj = JObject.Parse(@"{
              ""NestedObj"": {
                ""NullValue"": null,
                ""ObjValue"": {},
                ""ArrayValue"": [],
                ""StringValue"": ""Hello"",
                ""BoolValue"": true,
                ""IntValue"": 42,
                ""DoubleValue"": 4.5
              }
            }");
      IJSValueReader reader = new JTokenJSValueReader(jobj);
      JSValue jsValue = JSValue.ReadFrom(reader);

      Assert.AreEqual(JSValueType.Object, jsValue.Type, "tag_b101");
      Assert.AreEqual(JSValueType.Object, jsValue["NestedObj"].Type, "tag_b102");
      jsValue["NestedObj"].TryGetObject(out var nestedObj);

      Assert.AreEqual(JSValueType.Null, nestedObj["NullValue"].Type, "tag_b201");
      Assert.AreEqual(JSValueType.Object, nestedObj["ObjValue"].Type, "tag_b202");
      Assert.AreEqual(JSValueType.Array, nestedObj["ArrayValue"].Type, "tag_b203");
      Assert.AreEqual(JSValueType.String, nestedObj["StringValue"].Type, "tag_b204");
      Assert.AreEqual(JSValueType.Boolean, nestedObj["BoolValue"].Type, "tag_b205");
      Assert.AreEqual(JSValueType.Int64, nestedObj["IntValue"].Type, "tag_b206");
      Assert.AreEqual(JSValueType.Double, nestedObj["DoubleValue"].Type, "tag_b207");

      Assert.IsTrue(nestedObj["NullValue"].IsNull, "tag_b301");
      Assert.IsTrue(nestedObj["ObjValue"].TryGetObject(out var _), "tag_b302");
      Assert.IsTrue(nestedObj["ArrayValue"].TryGetArray(out var _), "tag_b303");
      Assert.AreEqual("Hello", nestedObj["StringValue"], "tag_b304");
      Assert.AreEqual(true, nestedObj["BoolValue"], "tag_b305");
      Assert.AreEqual(42, nestedObj["IntValue"], "tag_b306");
      Assert.AreEqual(4.5, nestedObj["DoubleValue"], "tag_b307");
    }

    [TestMethod]
    public void TestReadArray()
    {
      JArray jarr = JArray.Parse(@"[null, {}, [], ""Hello"", true, 42, 4.5]");
      IJSValueReader reader = new JTokenJSValueReader(jarr);
      JSValue jsValue = JSValue.ReadFrom(reader);

      Assert.AreEqual(JSValueType.Array, jsValue.Type, "tag_c101");
      Assert.AreEqual(JSValueType.Null, jsValue[0].Type, "tag_c102");
      Assert.AreEqual(JSValueType.Object, jsValue[1].Type, "tag_c103");
      Assert.AreEqual(JSValueType.Array, jsValue[2].Type, "tag_c104");
      Assert.AreEqual(JSValueType.String, jsValue[3].Type, "tag_c105");
      Assert.AreEqual(JSValueType.Boolean, jsValue[4].Type, "tag_c106");
      Assert.AreEqual(JSValueType.Int64, jsValue[5].Type, "tag_c107");
      Assert.AreEqual(JSValueType.Double, jsValue[6].Type, "tag_c108");

      Assert.IsTrue(jsValue[0].IsNull, "tag_c201");
      Assert.IsTrue(jsValue[1].TryGetObject(out var _), "tag_c202");
      Assert.IsTrue(jsValue[2].TryGetArray(out var _), "tag_c203");
      Assert.AreEqual("Hello", jsValue[3], "tag_c204");
      Assert.AreEqual(true, jsValue[4], "tag_c205");
      Assert.AreEqual(42, jsValue[5], "tag_c206");
      Assert.AreEqual(4.5, jsValue[6], "tag_c207");
    }

    [TestMethod]
    public void TestReadNestedArray()
    {
      JArray jarr = JArray.Parse(@"[[null, {}, [], ""Hello"", true, 42, 4.5]]");
      IJSValueReader reader = new JTokenJSValueReader(jarr);
      JSValue jsValue = JSValue.ReadFrom(reader);

      Assert.AreEqual(JSValueType.Array, jsValue.Type, "tag_d101");
      Assert.AreEqual(JSValueType.Array, jsValue[0].Type, "tag_d102");
      jsValue[0].TryGetArray(out var nestedArr);

      Assert.AreEqual(JSValueType.Null, nestedArr[0].Type, "tag_d201");
      Assert.AreEqual(JSValueType.Object, nestedArr[1].Type, "tag_d202");
      Assert.AreEqual(JSValueType.Array, nestedArr[2].Type, "tag_d203");
      Assert.AreEqual(JSValueType.String, nestedArr[3].Type, "tag_d204");
      Assert.AreEqual(JSValueType.Boolean, nestedArr[4].Type, "tag_d205");
      Assert.AreEqual(JSValueType.Int64, nestedArr[5].Type, "tag_d206");
      Assert.AreEqual(JSValueType.Double, nestedArr[6].Type, "tag_d207");

      Assert.IsTrue(nestedArr[0].IsNull, "tag_d301");
      Assert.IsTrue(nestedArr[1].TryGetObject(out var _), "tag_d302");
      Assert.IsTrue(nestedArr[2].TryGetArray(out var _), "tag_d303");
      Assert.AreEqual("Hello", nestedArr[3], "tag_d304");
      Assert.AreEqual(true, nestedArr[4], "tag_d305");
      Assert.AreEqual(42, nestedArr[5], "tag_d306");
      Assert.AreEqual(4.5, nestedArr[6], "tag_d307");
    }

    [TestMethod]
    public void TestJSSimpleLiterals()
    {
      JSValue jsValue01 = JSValue.Null;
      JSValue jsValue02 = "Hello";
      JSValue jsValue03 = "";
      JSValue jsValue04 = true;
      JSValue jsValue05 = false;
      JSValue jsValue06 = 42;
      JSValue jsValue07 = 0;
      JSValue jsValue08 = 4.5;
      JSValue jsValue09 = 0.0;
      JSValue jsValue10 = double.NaN;
      JSValue jsValue11 = double.PositiveInfinity;
      JSValue jsValue12 = double.NegativeInfinity;

      Assert.AreEqual(JSValueType.Null, jsValue01.Type, "tag_e101");
      Assert.AreEqual(JSValueType.String, jsValue02.Type, "tag_e102");
      Assert.AreEqual(JSValueType.String, jsValue03.Type, "tag_e103");
      Assert.AreEqual(JSValueType.Boolean, jsValue04.Type, "tag_e104");
      Assert.AreEqual(JSValueType.Boolean, jsValue05.Type, "tag_e105");
      Assert.AreEqual(JSValueType.Int64, jsValue06.Type, "tag_e106");
      Assert.AreEqual(JSValueType.Int64, jsValue07.Type, "tag_e107");
      Assert.AreEqual(JSValueType.Double, jsValue08.Type, "tag_e108");
      Assert.AreEqual(JSValueType.Double, jsValue09.Type, "tag_e109");
      Assert.AreEqual(JSValueType.Double, jsValue10.Type, "tag_e110");
      Assert.AreEqual(JSValueType.Double, jsValue11.Type, "tag_e111");
      Assert.AreEqual(JSValueType.Double, jsValue12.Type, "tag_e112");

      Assert.IsTrue(jsValue01.IsNull, "tag_e201");
      Assert.IsTrue(jsValue02.TryGetString(out var str1) && str1 == "Hello", "tag_e202");
      Assert.IsTrue(jsValue03.TryGetString(out var str2) && str2 == "", "tag_e203");
      Assert.IsTrue(jsValue04.TryGetBoolean(out var bool1) && bool1 == true, "tag_e204");
      Assert.IsTrue(jsValue05.TryGetBoolean(out var bool2) && bool2 == false, "tag_e205");
      Assert.IsTrue(jsValue06.TryGetInt64(out var int1) && int1 == 42, "tag_e206");
      Assert.IsTrue(jsValue07.TryGetInt64(out var int2) && int2 == 0, "tag_e207");
      Assert.IsTrue(jsValue08.TryGetDouble(out var double1) && double1 == 4.5, "tag_e208");
      Assert.IsTrue(jsValue09.TryGetDouble(out var double2) && double2 == 0, "tag_e209");
      Assert.IsTrue(jsValue10.TryGetDouble(out var double3) && double.IsNaN(double3), "tag_e210");
      Assert.IsTrue(jsValue11.TryGetDouble(out var double4) && double4 == double.PositiveInfinity, "tag_e211");
      Assert.IsTrue(jsValue12.TryGetDouble(out var double5) && double5 == double.NegativeInfinity, "tag_e212");
    }

    [TestMethod]
    public void TestObjectLiteral()
    {
      JSValue jsValue = new JSValueObject
      {
        ["NullValue"] = JSValue.Null,
        ["ObjValue"] = new JSValueObject { ["prop1"] = 2 },
        ["ObjValueEmpty"] = JSValue.EmptyObject,
        ["ArrayValue"] = new JSValueArray { 1, 2 },
        ["ArrayValueEmpty"] = JSValue.EmptyArray,
        ["StringValue"] = "Hello",
        ["BoolValue"] = true,
        ["IntValue"] = 42,
        ["DoubleValue"] = 4.5
      };

      Assert.AreEqual(JSValueType.Object, jsValue.Type, "tag_f101");
      Assert.AreEqual(JSValueType.Null, jsValue["NullValue"].Type, "tag_f102");
      Assert.AreEqual(JSValueType.Object, jsValue["ObjValue"].Type, "tag_f103");
      Assert.AreEqual(JSValueType.Object, jsValue["ObjValueEmpty"].Type, "tag_f104");
      Assert.AreEqual(JSValueType.Array, jsValue["ArrayValue"].Type, "tag_f105");
      Assert.AreEqual(JSValueType.Array, jsValue["ArrayValueEmpty"].Type, "tag_f106");
      Assert.AreEqual(JSValueType.String, jsValue["StringValue"].Type, "tag_f107");
      Assert.AreEqual(JSValueType.Boolean, jsValue["BoolValue"].Type, "tag_f108");
      Assert.AreEqual(JSValueType.Int64, jsValue["IntValue"].Type, "tag_f109");
      Assert.AreEqual(JSValueType.Double, jsValue["DoubleValue"].Type, "tag_f110");

      Assert.IsTrue(jsValue["NullValue"].IsNull, "tag_f201");
      Assert.AreEqual(1, jsValue["ObjValue"].PropertyCount, "tag_f202");
      Assert.AreEqual(2, jsValue["ObjValue"]["prop1"], "tag_f203");
      Assert.AreEqual(0, jsValue["ObjValueEmpty"].PropertyCount, "tag_f204");
      Assert.AreEqual(2, jsValue["ArrayValue"].ItemCount, "tag_f205");
      Assert.AreEqual(1, jsValue["ArrayValue"][0], "tag_f206");
      Assert.AreEqual(2, jsValue["ArrayValue"][1], "tag_f207");
      Assert.AreEqual(0, jsValue["ArrayValueEmpty"].ItemCount, "tag_f208");
      Assert.AreEqual("Hello", jsValue["StringValue"], "tag_f209");
      Assert.AreEqual(true, jsValue["BoolValue"], "tag_f210");
      Assert.AreEqual(42, jsValue["IntValue"], "tag_f211");
      Assert.AreNotEqual(24, jsValue["IntValue"], "tag_f212");
      Assert.AreEqual(4.5, jsValue["DoubleValue"], "tag_f213");
    }

    [TestMethod]
    public void TestArrayLiteral()
    {
      JSValue jsValue = new JSValueArray
      {
        JSValue.Null,
        new JSValueObject { ["prop1"] = 2 },
        JSValue.EmptyObject,
        new JSValueArray { 1, 2 },
        JSValue.EmptyArray,
        "Hello",
        true,
        42,
        4.5
      };

      Assert.AreEqual(JSValueType.Array, jsValue.Type, "tag_g101");
      Assert.AreEqual(JSValueType.Null, jsValue[0].Type, "tag_g102");
      Assert.AreEqual(JSValueType.Object, jsValue[1].Type, "tag_g103");
      Assert.AreEqual(JSValueType.Object, jsValue[2].Type, "tag_g104");
      Assert.AreEqual(JSValueType.Array, jsValue[3].Type, "tag_g105");
      Assert.AreEqual(JSValueType.Array, jsValue[4].Type, "tag_g106");
      Assert.AreEqual(JSValueType.String, jsValue[5].Type, "tag_g107");
      Assert.AreEqual(JSValueType.Boolean, jsValue[6].Type, "tag_g108");
      Assert.AreEqual(JSValueType.Int64, jsValue[7].Type, "tag_g109");
      Assert.AreEqual(JSValueType.Double, jsValue[8].Type, "tag_g110");

      Assert.IsTrue(jsValue["NullValue"].IsNull, "tag_g201");
      Assert.AreEqual(1, jsValue[1].PropertyCount, "tag_g202");
      Assert.AreEqual(2, jsValue[1]["prop1"], "tag_g203");
      Assert.AreEqual(0, jsValue[2].PropertyCount, "tag_g204");
      Assert.AreEqual(2, jsValue[3].ItemCount, "tag_g205");
      Assert.AreEqual(1, jsValue[3][0], "tag_g206");
      Assert.AreEqual(2, jsValue[3][1], "tag_g207");
      Assert.AreEqual(0, jsValue[4].ItemCount, "tag_g208");
      Assert.AreEqual("Hello", jsValue[5], "tag_g209");
      Assert.AreEqual(true, jsValue[6], "tag_g210");
      Assert.AreEqual(42, jsValue[7], "tag_g211");
      Assert.AreNotEqual(24, jsValue[7], "tag_g212");
      Assert.AreEqual(4.5, jsValue[8], "tag_g213");
    }

    [TestMethod]
    public void TestJSValueConstructor()
    {
      var value01 = new JSValue();
      var value02 = new JSValue(new JSValueObject { ["prop1"] = 3 });
      var value03 = new JSValue(new JSValueObject { });
      var value04 = new JSValue(new JSValueArray { 1, 2 });
      var value05 = new JSValue(new JSValueArray { });
      var value06 = new JSValue("Hello");
      var value07 = new JSValue(true);
      var value08 = new JSValue(false);
      var value09 = new JSValue(0);
      var value10 = new JSValue(42);
      var value11 = new JSValue(4.2);

      Assert.AreEqual(JSValueType.Null, value01.Type, "tag_h101");
      Assert.AreEqual(JSValueType.Object, value02.Type, "tag_h102");
      Assert.AreEqual(JSValueType.Object, value03.Type, "tag_h103");
      Assert.AreEqual(JSValueType.Array, value04.Type, "tag_h104");
      Assert.AreEqual(JSValueType.Array, value05.Type, "tag_h105");
      Assert.AreEqual(JSValueType.String, value06.Type, "tag_h106");
      Assert.AreEqual(JSValueType.Boolean, value07.Type, "tag_h107");
      Assert.AreEqual(JSValueType.Boolean, value08.Type, "tag_h108");
      Assert.AreEqual(JSValueType.Int64, value09.Type, "tag_h109");
      Assert.AreEqual(JSValueType.Int64, value10.Type, "tag_h110");
      Assert.AreEqual(JSValueType.Double, value11.Type, "tag_h111");

      Assert.IsTrue(value01.IsNull, "tag_h2001");
      Assert.IsTrue(value02.TryGetObject(out var objValue02) && objValue02.Count == 1, "tag_h202");
      Assert.IsTrue(value03.TryGetObject(out var objValue03) && objValue03.Count == 0, "tag_h203");
      Assert.IsTrue(value04.TryGetArray(out var arrValue04) && arrValue04.Count == 2, "tag_h204");
      Assert.IsTrue(value05.TryGetArray(out var arrValue05) && arrValue05.Count == 0, "tag_h205");
      Assert.IsTrue(value06.TryGetString(out var strValue06) && strValue06 == "Hello", "tag_h206");
      Assert.IsTrue(value07.TryGetBoolean(out var boolValue07) && boolValue07 == true, "tag_h207");
      Assert.IsTrue(value08.TryGetBoolean(out var boolValue08) && boolValue08 == false, "tag_h208");
      Assert.IsTrue(value09.TryGetInt64(out var intValue09) && intValue09 == 0, "tag_h209");
      Assert.IsTrue(value10.TryGetInt64(out var intValue10) && intValue10 == 42, "tag_h210");
      Assert.IsTrue(value11.TryGetDouble(out var doubleValue11) && doubleValue11 == 4.2, "tag_h211");
    }

    [TestMethod]
    public void TestJSValueImplicitCast()
    {
      JSValue value01 = new ReadOnlyDictionary<string, JSValue>(new Dictionary<string, JSValue> { ["prop1"] = 3 });
      JSValue value02 = new Dictionary<string, JSValue> { ["prop1"] = 3 };
      JSValue value03 = new JSValueObject { ["prop1"] = 3 };
      JSValue value04 = new ReadOnlyCollection<JSValue>(new List<JSValue> { 1, 2 });
      JSValue value05 = new List<JSValue> { 1, 2 };
      JSValue value06 = new JSValueArray { 1, 2 };
      JSValue value07 = "Hello";
      JSValue value08 = true;
      JSValue value09 = false;
      JSValue value10 = (sbyte)42;
      JSValue value11 = (short)42;
      JSValue value12 = 42;
      JSValue value13 = (long)42;
      JSValue value14 = (byte)42;
      JSValue value15 = (ushort)42;
      JSValue value16 = (uint)42;
      JSValue value17 = (ulong)42;
      JSValue value18 = (float)4.2;
      JSValue value19 = 4.2;

      Assert.AreEqual(JSValueType.Object, value01.Type, "tag_i101");
      Assert.AreEqual(JSValueType.Object, value02.Type, "tag_i102");
      Assert.AreEqual(JSValueType.Object, value03.Type, "tag_i103");
      Assert.AreEqual(JSValueType.Array, value04.Type, "tag_i104");
      Assert.AreEqual(JSValueType.Array, value05.Type, "tag_i105");
      Assert.AreEqual(JSValueType.Array, value06.Type, "tag_i106");
      Assert.AreEqual(JSValueType.String, value07.Type, "tag_i107");
      Assert.AreEqual(JSValueType.Boolean, value08.Type, "tag_i108");
      Assert.AreEqual(JSValueType.Boolean, value09.Type, "tag_i109");
      Assert.AreEqual(JSValueType.Int64, value10.Type, "tag_i110");
      Assert.AreEqual(JSValueType.Int64, value11.Type, "tag_i111");
      Assert.AreEqual(JSValueType.Int64, value12.Type, "tag_i112");
      Assert.AreEqual(JSValueType.Int64, value13.Type, "tag_i113");
      Assert.AreEqual(JSValueType.Int64, value14.Type, "tag_i114");
      Assert.AreEqual(JSValueType.Int64, value15.Type, "tag_i115");
      Assert.AreEqual(JSValueType.Int64, value16.Type, "tag_i116");
      Assert.AreEqual(JSValueType.Int64, value17.Type, "tag_i117");
      Assert.AreEqual(JSValueType.Double, value18.Type, "tag_i118");
      Assert.AreEqual(JSValueType.Double, value19.Type, "tag_i119");

      Assert.IsTrue(value01.TryGetObject(out var objValue01) && objValue01.Count == 1, "tag_i201");
      Assert.IsTrue(value02.TryGetObject(out var objValue02) && objValue02.Count == 1, "tag_i202");
      Assert.IsTrue(value03.TryGetObject(out var objValue03) && objValue03.Count == 1, "tag_i203");
      Assert.IsTrue(value04.TryGetArray(out var arrValue04) && arrValue04.Count == 2, "tag_i204");
      Assert.IsTrue(value05.TryGetArray(out var arrValue05) && arrValue05.Count == 2, "tag_i205");
      Assert.IsTrue(value06.TryGetArray(out var arrValue06) && arrValue06.Count == 2, "tag_i206");
      Assert.IsTrue(value07.TryGetString(out var strValue07) && strValue07 == "Hello", "tag_i207");
      Assert.IsTrue(value08.TryGetBoolean(out var boolValue08) && boolValue08 == true, "tag_i208");
      Assert.IsTrue(value09.TryGetBoolean(out var boolValue09) && boolValue09 == false, "tag_i209");
      Assert.IsTrue(value10.TryGetInt64(out var intValue10) && intValue10 == 42, "tag_i210");
      Assert.IsTrue(value11.TryGetInt64(out var intValue11) && intValue11 == 42, "tag_i211");
      Assert.IsTrue(value12.TryGetInt64(out var intValue12) && intValue12 == 42, "tag_i212");
      Assert.IsTrue(value13.TryGetInt64(out var intValue13) && intValue13 == 42, "tag_i213");
      Assert.IsTrue(value14.TryGetInt64(out var intValue14) && intValue14 == 42, "tag_i214");
      Assert.IsTrue(value15.TryGetInt64(out var intValue15) && intValue15 == 42, "tag_i215");
      Assert.IsTrue(value16.TryGetInt64(out var intValue16) && intValue16 == 42, "tag_i216");
      Assert.IsTrue(value17.TryGetInt64(out var intValue17) && intValue17 == 42, "tag_i217");
      Assert.IsTrue(value18.TryGetDouble(out var doubleValue18) && doubleValue18 == (float)4.2, "tag_i218");
      Assert.IsTrue(value19.TryGetDouble(out var doubleValue19) && doubleValue19 == 4.2, "tag_i219");
    }

    [TestMethod]
    public void TestAsObject()
    {
      // Any type except for Object is returned as EmptyObject.
      bool AsObjectIsEmpty(JSValue value) => value.AsObject().Count == 0;

      Assert.IsFalse(AsObjectIsEmpty(new JSValueObject { ["prop1"] = 42 }), "tag_j01");
      Assert.IsTrue(AsObjectIsEmpty(JSValue.EmptyObject), "tag_j02");
      Assert.IsTrue(AsObjectIsEmpty(new JSValueArray { 42, 78 }), "tag_j03");
      Assert.IsTrue(AsObjectIsEmpty(JSValue.EmptyArray), "tag_j04");
      Assert.IsTrue(AsObjectIsEmpty(""), "tag_j05");
      Assert.IsTrue(AsObjectIsEmpty("Hello"), "tag_j06");
      Assert.IsTrue(AsObjectIsEmpty(true), "tag_j07");
      Assert.IsTrue(AsObjectIsEmpty(false), "tag_j08");
      Assert.IsTrue(AsObjectIsEmpty(0), "tag_j09");
      Assert.IsTrue(AsObjectIsEmpty(42), "tag_j10");
      Assert.IsTrue(AsObjectIsEmpty(long.MaxValue), "tag_j11");
      Assert.IsTrue(AsObjectIsEmpty(long.MinValue), "tag_j12");
      Assert.IsTrue(AsObjectIsEmpty(0.0), "tag_j13");
      Assert.IsTrue(AsObjectIsEmpty(4.2), "tag_j14");
      Assert.IsTrue(AsObjectIsEmpty(double.NaN), "tag_j15");
      Assert.IsTrue(AsObjectIsEmpty(double.PositiveInfinity), "tag_j16");
      Assert.IsTrue(AsObjectIsEmpty(double.NegativeInfinity), "tag_j17");
      Assert.IsTrue(AsObjectIsEmpty(JSValue.Null), "tag_j18");
    }

    [TestMethod]
    public void TestAsArray()
    {
      // Any type except for Array is returned as EmptyObject.
      bool AsArrayIsEmpty(JSValue value) => value.AsArray().Count == 0;

      Assert.IsTrue(AsArrayIsEmpty(new JSValueObject { ["prop1"] = 42 }), "tag_k01");
      Assert.IsTrue(AsArrayIsEmpty(JSValue.EmptyObject), "tag_k02");
      Assert.IsFalse(AsArrayIsEmpty(new JSValueArray { 42, 78 }), "tag_k03");
      Assert.IsTrue(AsArrayIsEmpty(JSValue.EmptyArray), "tag_k04");
      Assert.IsTrue(AsArrayIsEmpty(""), "tag_k05");
      Assert.IsTrue(AsArrayIsEmpty("Hello"), "tag_k06");
      Assert.IsTrue(AsArrayIsEmpty(true), "tag_k07");
      Assert.IsTrue(AsArrayIsEmpty(false), "tag_k08");
      Assert.IsTrue(AsArrayIsEmpty(0), "tag_k09");
      Assert.IsTrue(AsArrayIsEmpty(42), "tag_k10");
      Assert.IsTrue(AsArrayIsEmpty(long.MaxValue), "tag_k11");
      Assert.IsTrue(AsArrayIsEmpty(long.MinValue), "tag_k12");
      Assert.IsTrue(AsArrayIsEmpty(0.0), "tag_k13");
      Assert.IsTrue(AsArrayIsEmpty(4.2), "tag_k14");
      Assert.IsTrue(AsArrayIsEmpty(double.NaN), "tag_k15");
      Assert.IsTrue(AsArrayIsEmpty(double.PositiveInfinity), "tag_k16");
      Assert.IsTrue(AsArrayIsEmpty(double.NegativeInfinity), "tag_k17");
      Assert.IsTrue(AsArrayIsEmpty(JSValue.Null), "tag_k18");
    }

    [TestMethod]
    public void TestAsConverters()
    {
      // Check AsString, AsBoolean, AsInt64, and AsDouble conversions.
      void CheckAsConverter(JSValue value, string asString, bool asBoolean, long asInt64, double asDouble, string tag)
      {
        Assert.AreEqual(asString, value.AsString(), "AsString: {0}", tag);
        Assert.AreEqual(asBoolean, value.AsBoolean(), "AsBoolean: {0}", tag);
        Assert.AreEqual(asInt64, value.AsInt64(), "AsInt64: {0}", tag);
        Assert.AreEqual(asDouble, value.AsDouble(), "AsDouble: {0}", tag);

        // Explicit cast is an alternative to the As conversion.
        Assert.AreEqual(asString, (string)value, "(string): {0}", tag);
        Assert.AreEqual(asBoolean, (bool)value, "(bool): {0}", tag);
        Assert.AreEqual(asInt64, (long)value, "(long): {0}", tag);
        Assert.AreEqual(asDouble, (double)value, "(double): {0}", tag);
      }

      CheckAsConverter(new JSValueObject { ["prop1"] = 42 }, "", true, 0, 0, "tag_l01");
      CheckAsConverter(JSValue.EmptyObject, "", false, 0, 0, "tag_l02");
      CheckAsConverter(new JSValueArray { 42, 78 }, "", true, 0, 0, "tag_l03");
      CheckAsConverter(JSValue.EmptyArray, "", false, 0, 0, "tag_l04");
      CheckAsConverter("", "", false, 0, 0, "tag_l05");
      CheckAsConverter("  ", "  ", false, 0, 0, "tag_l06");
      CheckAsConverter("42", "42", false, 42, 42, "tag_l07");
      CheckAsConverter("  42  ", "  42  ", false, 42, 42, "tag_l08");
      CheckAsConverter("4.2", "4.2", false, 4, 4.2, "tag_l09");
      CheckAsConverter("Hello", "Hello", false, 0, double.NaN, "tag_l10");
      CheckAsConverter("true", "true", true, 0, double.NaN, "tag_l11");
      CheckAsConverter("false", "false", false, 0, double.NaN, "tag_l12");
      CheckAsConverter("True", "True", true, 0, double.NaN, "tag_l13");
      CheckAsConverter("False", "False", false, 0, double.NaN, "tag_l14");
      CheckAsConverter("TRUE", "TRUE", true, 0, double.NaN, "tag_l15");
      CheckAsConverter("FALSE", "FALSE", false, 0, double.NaN, "tag_l16");
      CheckAsConverter("on", "on", true, 0, double.NaN, "tag_l17");
      CheckAsConverter("off", "off", false, 0, double.NaN, "tag_l18");
      CheckAsConverter("On", "On", true, 0, double.NaN, "tag_l19");
      CheckAsConverter("Off", "Off", false, 0, double.NaN, "tag_l20");
      CheckAsConverter("ON", "ON", true, 0, double.NaN, "tag_l21");
      CheckAsConverter("OFF", "OFF", false, 0, double.NaN, "tag_l22");
      CheckAsConverter("yes", "yes", true, 0, double.NaN, "tag_l23");
      CheckAsConverter("no", "no", false, 0, double.NaN, "tag_l24");
      CheckAsConverter("y", "y", true, 0, double.NaN, "tag_l25");
      CheckAsConverter("n", "n", false, 0, double.NaN, "tag_l26");
      CheckAsConverter("Y", "Y", true, 0, double.NaN, "tag_l27");
      CheckAsConverter("N", "N", false, 0, double.NaN, "tag_l28");
      CheckAsConverter("1", "1", true, 1, 1, "tag_l29");
      CheckAsConverter("0", "0", false, 0, 0, "tag_l20");
      CheckAsConverter(true, "true", true, 1, 1, "tag_l31");
      CheckAsConverter(false, "false", false, 0, 0, "tag_l32");
      CheckAsConverter(0, "0", false, 0, 0, "tag_l33");
      CheckAsConverter(42, "42", true, 42, 42, "tag_l34");
      CheckAsConverter(long.MaxValue, "9223372036854775807", true, long.MaxValue, long.MaxValue, "tag_l35");
      CheckAsConverter(long.MinValue, "-9223372036854775808", true, long.MinValue, long.MinValue, "tag_l36");
      CheckAsConverter(0.0, "0", false, 0, 0, "tag_l37");
      CheckAsConverter(4.2, "4.2", true, 4, 4.2, "tag_l38");
      CheckAsConverter(-4.2, "-4.2", true, -4, -4.2, "tag_l39");
      CheckAsConverter(double.MaxValue, "1.79769313486232E+308", true, 0, double.MaxValue, "tag_l40");
      CheckAsConverter(double.MinValue, "-1.79769313486232E+308", true, 0, double.MinValue, "tag_l41");
      CheckAsConverter(double.NaN, "NaN", false, 0, double.NaN, "tag_l42");
      CheckAsConverter(double.PositiveInfinity, "Infinity", true, 0, double.PositiveInfinity, "tag_l43");
      CheckAsConverter(double.NegativeInfinity, "-Infinity", true, 0, double.NegativeInfinity, "tag_l44");
      CheckAsConverter(JSValue.Null, "null", false, 0, 0, "tag_l45");
    }

    [TestMethod]
    public void TestExplicitNumberConversion()
    {
      // Check that explicit number conversions are defined
      Assert.AreEqual(42, (sbyte)new JSValue(42), "tag_m01");
      Assert.AreEqual(42, (short)new JSValue(42), "tag_m02");
      Assert.AreEqual(42, (int)new JSValue(42), "tag_m03");
      Assert.AreEqual(42, (long)new JSValue(42), "tag_m04");
      Assert.AreEqual(42, (byte)new JSValue(42), "tag_m05");
      Assert.AreEqual(42, (ushort)new JSValue(42), "tag_m06");
      Assert.AreEqual(42u, (uint)new JSValue(42), "tag_m07");
      Assert.AreEqual(42u, (ulong)new JSValue(42), "tag_m08");
      Assert.AreEqual((float)4.2, (float)new JSValue(4.2), "tag_m09");
      Assert.AreEqual(4.2, (double)new JSValue(4.2), "tag_m10");
    }

    [TestMethod]
    public void TestAsJSConverters()
    {
      // Check AsJSString, AsJSBoolean, and AsJSNumber conversions.
      // They must match the JavaScript String(), Boolean(), and Number() conversions. 
      void CheckAsJSConverter(JSValue value, string asJSString, bool asJSBoolean, double asJSNumber, string tag)
      {
        Assert.AreEqual(asJSString, value.AsJSString(), "AsJSString: {0}", tag);
        Assert.AreEqual(asJSBoolean, value.AsJSBoolean(), "AsJSBoolean: {0}", tag);
        Assert.AreEqual(asJSNumber, value.AsJSNumber(), "AsJSNumber: {0}", tag);
      }

      CheckAsJSConverter(false, "false", false, 0, "tag_n01");
      CheckAsJSConverter(true, "true", true, 1, "tag_n02");
      CheckAsJSConverter(0, "0", false, 0, "tag_n03");
      CheckAsJSConverter(1, "1", true, 1, "tag_n04");
      CheckAsJSConverter("0", "0", true, 0, "tag_n05");
      CheckAsJSConverter("000", "000", true, 0, "tag_n06");
      CheckAsJSConverter("1", "1", true, 1, "tag_n07");
      CheckAsJSConverter(double.NaN, "NaN", false, double.NaN, "tag_n08");
      CheckAsJSConverter(double.PositiveInfinity, "Infinity", true, double.PositiveInfinity, "tag_n09");
      CheckAsJSConverter(double.NegativeInfinity, "-Infinity", true, double.NegativeInfinity, "tag_n10");
      CheckAsJSConverter("", "", false, 0, "tag_n11");
      CheckAsJSConverter("20", "20", true, 20, "tag_n12");
      CheckAsJSConverter("twenty", "twenty", true, double.NaN, "tag_n13");
      CheckAsJSConverter(new JSValueArray { }, "", true, 0, "tag_n14");
      CheckAsJSConverter(new JSValueArray { 20 }, "20", true, 20, "tag_n15");
      CheckAsJSConverter(new JSValueArray { 10, 20 }, "10,20", true, double.NaN, "tag_n16");
      CheckAsJSConverter(new JSValueArray { "twenty" }, "twenty", true, double.NaN, "tag_n17");
      CheckAsJSConverter(new JSValueArray { "ten", "twenty" }, "ten,twenty", true, double.NaN, "tag_n18");
      CheckAsJSConverter(new JSValueArray { double.NaN }, "NaN", true, double.NaN, "tag_n19");
      CheckAsJSConverter(new JSValueObject { }, "[object Object]", true, double.NaN, "tag_n20");
      CheckAsJSConverter(JSValue.Null, "null", false, 0, "tag_n21");
    }

    [TestMethod]
    public void TestJSEquals()
    {
      Assert.IsTrue(new JSValue().JSEquals(JSValue.Null), "tag_o1001");
      Assert.IsFalse(new JSValue().JSEquals(new JSValueObject { }), "tag_o1002");
      Assert.IsFalse(new JSValue().JSEquals(new JSValueArray { }), "tag_o1003");
      Assert.IsFalse(new JSValue().JSEquals(""), "tag_o1004");
      Assert.IsFalse(new JSValue().JSEquals(false), "tag_o1005");
      Assert.IsFalse(new JSValue().JSEquals(0), "tag_o1006");
      Assert.IsFalse(new JSValue().JSEquals(0.0), "tag_o1007");

      Assert.IsFalse(new JSValue(new JSValueObject()).JSEquals(JSValue.Null), "tag_o2001");
      Assert.IsTrue(new JSValue(new JSValueObject { }).JSEquals(new JSValueObject { }), "tag_o2002");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { }), "tag_o2003");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { "Hello" }), "tag_o2004");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { 0 }), "tag_o2005");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { "0" }), "tag_o2006");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { 1 }), "tag_o2007");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { "1" }), "tag_o2008");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { true }), "tag_o2009");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(new JSValueArray { "true" }), "tag_o2010");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(""), "tag_o2011");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals("0"), "tag_o2012");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals("1"), "tag_o2013");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals("true"), "tag_o2014");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals("false"), "tag_o2015");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals("Hello"), "tag_o2016");
      Assert.IsTrue(new JSValue(new JSValueObject { }).JSEquals("[object Object]"), "tag_o2017");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(false), "tag_o2018");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(true), "tag_o2019");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(0), "tag_o2020");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(5), "tag_o2021");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(1), "tag_o2022");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(0.0), "tag_o2023");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(0.5), "tag_o2024");
      Assert.IsFalse(new JSValue(new JSValueObject { }).JSEquals(1.0), "tag_o2025");

      Assert.IsTrue(new JSValue(new JSValueObject { { "prop1", 2 }, { "prop2", false } })
                      .JSEquals(new JSValueObject { { "prop2", 0 }, { "prop1", "2" } }), "tag_o3001");
      Assert.IsFalse(new JSValue(new JSValueObject { { "prop1", 2 }, { "prop2", false } })
                       .JSEquals(new JSValueObject { { "prop2", 0 } }), "tag_o3002");
      Assert.IsFalse(new JSValue(new JSValueObject { { "prop1", 2 }, { "prop2", false } })
                       .JSEquals(new JSValueObject { { "prop1", 2 }, { "prop25", false } }), "tag_o3003");
      Assert.IsFalse(new JSValue(new JSValueObject { { "prop1", 2 }, { "prop2", false } })
                       .JSEquals(new JSValueObject { { "prop1", 2 }, { "prop2", true } }), "tag_o3004");

      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(JSValue.Null), "tag_o4001");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueObject { }), "tag_o4002");
      Assert.IsTrue(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { }), "tag_o4003");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { "Hello" }), "tag_o4004");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { 0 }), "tag_o4005");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { "0" }), "tag_o4006");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { 1 }), "tag_o4007");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { "1" }), "tag_o4008");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { true }), "tag_o4009");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(new JSValueArray { "true" }), "tag_o4010");
      Assert.IsTrue(new JSValue(new JSValueArray { }).JSEquals(""), "tag_o4011");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals("0"), "tag_o4012");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals("1"), "tag_o4013");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals("true"), "tag_o4014");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals("false"), "tag_o4015");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals("Hello"), "tag_o4016");
      Assert.IsTrue(new JSValue(new JSValueArray { }).JSEquals(false), "tag_o4017");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(true), "tag_o4018");
      Assert.IsTrue(new JSValue(new JSValueArray { }).JSEquals(0), "tag_o4019");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(5), "tag_o4020");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(1), "tag_o4021");
      Assert.IsTrue(new JSValue(new JSValueArray { }).JSEquals(0.0), "tag_o4022");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(0.5), "tag_o4023");
      Assert.IsFalse(new JSValue(new JSValueArray { }).JSEquals(1.0), "tag_o4024");

      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(JSValue.Null), "tag_o5001");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueObject { }), "tag_o5002");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { }), "tag_o5003");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { "Hello" }), "tag_o5004");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { 0 }), "tag_o5005");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { "0" }), "tag_o5006");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { 1 }), "tag_o5007");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { "1" }), "tag_o5008");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { true }), "tag_o5009");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(new JSValueArray { "true" }), "tag_o5010");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(""), "tag_o5011");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals("0"), "tag_o5012");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals("1"), "tag_o5013");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals("true"), "tag_o5014");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals("false"), "tag_o5015");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals("Hello"), "tag_o5016");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(false), "tag_o5017");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(true), "tag_o5018");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(0), "tag_o5019");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(5), "tag_o5020");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(1), "tag_o5021");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(0.0), "tag_o5022");
      Assert.IsFalse(new JSValue(new JSValueArray { 1 }).JSEquals(0.5), "tag_o5023");
      Assert.IsTrue(new JSValue(new JSValueArray { 1 }).JSEquals(1.0), "tag_o5024");

      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(JSValue.Null), "tag_o6001");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueObject { }), "tag_o6002");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { }), "tag_o6003");
      Assert.IsTrue(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { "Hello" }), "tag_o6004");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { 0 }), "tag_o6005");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { "0" }), "tag_o6006");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { 1 }), "tag_o6007");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { "1" }), "tag_o6008");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { true }), "tag_o6009");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(new JSValueArray { "true" }), "tag_o6010");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(""), "tag_o6011");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals("0"), "tag_o6012");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals("1"), "tag_o6013");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals("true"), "tag_o6014");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals("false"), "tag_o6015");
      Assert.IsTrue(new JSValue(new JSValueArray { "Hello" }).JSEquals("Hello"), "tag_o6016");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(false), "tag_o6017");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(true), "tag_o6018");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(0), "tag_o6019");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(5), "tag_o6020");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(1), "tag_o6021");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(0.0), "tag_o6022");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(0.5), "tag_o6023");
      Assert.IsFalse(new JSValue(new JSValueArray { "Hello" }).JSEquals(1.0), "tag_o6024");

      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(JSValue.Null), "tag_o7001");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueObject { }), "tag_o7002");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { }), "tag_o7003");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "Hello" }), "tag_o7004");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { 0 }), "tag_o7005");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "0" }), "tag_o7006");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { 1 }), "tag_o7007");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "1" }), "tag_o7008");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { true }), "tag_o7009");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "true" }), "tag_o7010");
      Assert.IsTrue(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { 0, 1 }), "tag_o7011");
      Assert.IsTrue(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { false, true }), "tag_o7012");
      Assert.IsTrue(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "0", "1" }), "tag_o7013");
      Assert.IsTrue(new JSValue(new JSValueArray { 0, 1 }).JSEquals(new JSValueArray { "0", true }), "tag_o7014");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(""), "tag_o7015");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals("0"), "tag_o7016");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals("1"), "tag_o7017");
      Assert.IsTrue(new JSValue(new JSValueArray { 0, 1 }).JSEquals("0,1"), "tag_o7018");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals("true"), "tag_o7019");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals("false"), "tag_o7020");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals("Hello"), "tag_o7021");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(false), "tag_o7022");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(true), "tag_o7023");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(0), "tag_o7024");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(5), "tag_o7025");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(1), "tag_o7026");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(0.0), "tag_o7027");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(0.5), "tag_o7028");
      Assert.IsFalse(new JSValue(new JSValueArray { 0, 1 }).JSEquals(1.0), "tag_o7029");

      Assert.IsFalse(new JSValue("").JSEquals(JSValue.Null), "tag_o8001");
      Assert.IsFalse(new JSValue("").JSEquals(new JSValueObject { }), "tag_o8002");
      Assert.IsTrue(new JSValue("").JSEquals(new JSValueArray { }), "tag_o8003");
      Assert.IsFalse(new JSValue("").JSEquals(new JSValueArray { 0 }), "tag_o8004");
      Assert.IsFalse(new JSValue("").JSEquals(new JSValueArray { 1 }), "tag_o8005");
      Assert.IsFalse(new JSValue("").JSEquals(new JSValueArray { true }), "tag_o8006");
      Assert.IsTrue(new JSValue("").JSEquals(new JSValueArray { "" }), "tag_o8007");
      Assert.IsTrue(new JSValue("").JSEquals(""), "tag_o8008");
      Assert.IsFalse(new JSValue("").JSEquals("1"), "tag_o8009");
      Assert.IsFalse(new JSValue("").JSEquals("Hello"), "tag_o8010");
      Assert.IsTrue(new JSValue("").JSEquals(false), "tag_o8011");
      Assert.IsFalse(new JSValue("").JSEquals(true), "tag_o8012");
      Assert.IsTrue(new JSValue("").JSEquals(0), "tag_o8013");
      Assert.IsFalse(new JSValue("").JSEquals(5), "tag_o8014");
      Assert.IsFalse(new JSValue("").JSEquals(1), "tag_o8015");
      Assert.IsTrue(new JSValue("").JSEquals(0.0), "tag_o8016");
      Assert.IsFalse(new JSValue("").JSEquals(0.5), "tag_o8017");
      Assert.IsFalse(new JSValue("").JSEquals(1.0), "tag_o8018");

      Assert.IsFalse(new JSValue("Hello").JSEquals(JSValue.Null), "tag_o9001");
      Assert.IsFalse(new JSValue("Hello").JSEquals(new JSValueObject { }), "tag_o9002");
      Assert.IsFalse(new JSValue("Hello").JSEquals(new JSValueArray { }), "tag_o9003");
      Assert.IsFalse(new JSValue("Hello").JSEquals(new JSValueArray { 0 }), "tag_o9004");
      Assert.IsFalse(new JSValue("Hello").JSEquals(new JSValueArray { 1 }), "tag_o9005");
      Assert.IsFalse(new JSValue("Hello").JSEquals(new JSValueArray { true }), "tag_o9006");
      Assert.IsTrue(new JSValue("Hello").JSEquals(new JSValueArray { "Hello" }), "tag_o9007");
      Assert.IsFalse(new JSValue("Hello").JSEquals(""), "tag_o9008");
      Assert.IsFalse(new JSValue("Hello").JSEquals("1"), "tag_o9009");
      Assert.IsTrue(new JSValue("Hello").JSEquals("Hello"), "tag_o9010");
      Assert.IsFalse(new JSValue("Hello").JSEquals(false), "tag_o9011");
      Assert.IsFalse(new JSValue("Hello").JSEquals(true), "tag_o9012");
      Assert.IsFalse(new JSValue("Hello").JSEquals(0), "tag_o9013");
      Assert.IsFalse(new JSValue("Hello").JSEquals(5), "tag_o9014");
      Assert.IsFalse(new JSValue("Hello").JSEquals(1), "tag_o9015");
      Assert.IsFalse(new JSValue("Hello").JSEquals(0.0), "tag_o9016");
      Assert.IsFalse(new JSValue("Hello").JSEquals(0.5), "tag_o9017");
      Assert.IsFalse(new JSValue("Hello").JSEquals(1.0), "tag_o9018");

      Assert.IsFalse(new JSValue("0").JSEquals(JSValue.Null), "tag_o10001");
      Assert.IsFalse(new JSValue("0").JSEquals(new JSValueObject { }), "tag_o10002");
      Assert.IsFalse(new JSValue("0").JSEquals(new JSValueArray { }), "tag_o10003");
      Assert.IsFalse(new JSValue("0").JSEquals(new JSValueArray { "Hello" }), "tag_o10004");
      Assert.IsTrue(new JSValue("0").JSEquals(new JSValueArray { "0" }), "tag_o10005");
      Assert.IsTrue(new JSValue("0").JSEquals(new JSValueArray { 0 }), "tag_o10006");
      Assert.IsFalse(new JSValue("0").JSEquals(""), "tag_o10007");
      Assert.IsTrue(new JSValue("0").JSEquals("0"), "tag_o10008");
      Assert.IsFalse(new JSValue("0").JSEquals("Hello"), "tag_o10009");
      Assert.IsTrue(new JSValue("0").JSEquals(false), "tag_o10010");
      Assert.IsFalse(new JSValue("0").JSEquals(true), "tag_o10011");
      Assert.IsTrue(new JSValue("0").JSEquals(0), "tag_o10012");
      Assert.IsFalse(new JSValue("0").JSEquals(5), "tag_o10013");
      Assert.IsFalse(new JSValue("0").JSEquals(1), "tag_o10014");
      Assert.IsTrue(new JSValue("0").JSEquals(0.0), "tag_o10015");
      Assert.IsFalse(new JSValue("0").JSEquals(0.5), "tag_o10016");
      Assert.IsFalse(new JSValue("0").JSEquals(1.0), "tag_o10017");

      Assert.IsFalse(new JSValue("1").JSEquals(JSValue.Null), "tag_o11001");
      Assert.IsFalse(new JSValue("1").JSEquals(new JSValueObject { }), "tag_o11002");
      Assert.IsFalse(new JSValue("1").JSEquals(new JSValueArray { }), "tag_o11003");
      Assert.IsFalse(new JSValue("1").JSEquals(new JSValueArray { "Hello" }), "tag_o11004");
      Assert.IsTrue(new JSValue("1").JSEquals(new JSValueArray { "1" }), "tag_o11005");
      Assert.IsTrue(new JSValue("1").JSEquals(new JSValueArray { 1 }), "tag_o11006");
      Assert.IsFalse(new JSValue("1").JSEquals(""), "tag_o11007");
      Assert.IsTrue(new JSValue("1").JSEquals("1"), "tag_o11008");
      Assert.IsFalse(new JSValue("1").JSEquals("Hello"), "tag_o11009");
      Assert.IsFalse(new JSValue("1").JSEquals(false), "tag_o11010");
      Assert.IsTrue(new JSValue("1").JSEquals(true), "tag_o11011");
      Assert.IsFalse(new JSValue("1").JSEquals(0), "tag_o11012");
      Assert.IsFalse(new JSValue("1").JSEquals(5), "tag_o11013");
      Assert.IsTrue(new JSValue("1").JSEquals(1), "tag_o11014");
      Assert.IsFalse(new JSValue("1").JSEquals(0.0), "tag_o11015");
      Assert.IsFalse(new JSValue("1").JSEquals(0.5), "tag_o11016");
      Assert.IsTrue(new JSValue("1").JSEquals(1.0), "tag_o11017");

      Assert.IsFalse(new JSValue("true").JSEquals(JSValue.Null), "tag_o12001");
      Assert.IsFalse(new JSValue("true").JSEquals(new JSValueObject { }), "tag_o12002");
      Assert.IsFalse(new JSValue("true").JSEquals(new JSValueArray { }), "tag_o12003");
      Assert.IsFalse(new JSValue("true").JSEquals(new JSValueArray { 0 }), "tag_o12004");
      Assert.IsFalse(new JSValue("true").JSEquals(new JSValueArray { 1 }), "tag_o12005");
      Assert.IsTrue(new JSValue("true").JSEquals(new JSValueArray { true }), "tag_o12006");
      Assert.IsTrue(new JSValue("true").JSEquals(new JSValueArray { "true" }), "tag_o12007");
      Assert.IsFalse(new JSValue("true").JSEquals(""), "tag_o12008");
      Assert.IsFalse(new JSValue("true").JSEquals("1"), "tag_o12009");
      Assert.IsFalse(new JSValue("true").JSEquals("Hello"), "tag_o12010");
      Assert.IsTrue(new JSValue("true").JSEquals("true"), "tag_o12011");
      Assert.IsFalse(new JSValue("true").JSEquals(false), "tag_o12012");
      Assert.IsFalse(new JSValue("true").JSEquals(true), "tag_o12013");
      Assert.IsFalse(new JSValue("true").JSEquals(0), "tag_o12014");
      Assert.IsFalse(new JSValue("true").JSEquals(5), "tag_o12015");
      Assert.IsFalse(new JSValue("true").JSEquals(1), "tag_o12016");
      Assert.IsFalse(new JSValue("true").JSEquals(0.0), "tag_o12017");
      Assert.IsFalse(new JSValue("true").JSEquals(0.5), "tag_o12018");
      Assert.IsFalse(new JSValue("true").JSEquals(1.0), "tag_o12019");

      Assert.IsTrue(new JSValue("[object Object]").JSEquals(new JSValueObject { }), "tag_o13001");

      Assert.IsFalse(new JSValue(true).JSEquals(JSValue.Null), "tag_o14001");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueObject { }), "tag_o14002");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { }), "tag_o14003");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { "Hello" }), "tag_o14004");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { 0 }), "tag_o14005");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { "0" }), "tag_o14006");
      Assert.IsTrue(new JSValue(true).JSEquals(new JSValueArray { 1 }), "tag_o14007");
      Assert.IsTrue(new JSValue(true).JSEquals(new JSValueArray { "1" }), "tag_o14008");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { true }), "tag_o14009");
      Assert.IsFalse(new JSValue(true).JSEquals(new JSValueArray { "true" }), "tag_o14010");
      Assert.IsFalse(new JSValue(true).JSEquals(""), "tag_o14011");
      Assert.IsFalse(new JSValue(true).JSEquals("0"), "tag_o14012");
      Assert.IsTrue(new JSValue(true).JSEquals("1"), "tag_o14013");
      Assert.IsFalse(new JSValue(true).JSEquals("true"), "tag_o14014");
      Assert.IsFalse(new JSValue(true).JSEquals("false"), "tag_o14015");
      Assert.IsFalse(new JSValue(true).JSEquals("Hello"), "tag_o14016");
      Assert.IsFalse(new JSValue(true).JSEquals(false), "tag_o14017");
      Assert.IsTrue(new JSValue(true).JSEquals(true), "tag_o14018");
      Assert.IsFalse(new JSValue(true).JSEquals(0), "tag_o14019");
      Assert.IsFalse(new JSValue(true).JSEquals(5), "tag_o14020");
      Assert.IsTrue(new JSValue(true).JSEquals(1), "tag_o14021");
      Assert.IsFalse(new JSValue(true).JSEquals(0.0), "tag_o14022");
      Assert.IsFalse(new JSValue(true).JSEquals(0.5), "tag_o14023");
      Assert.IsTrue(new JSValue(true).JSEquals(1.0), "tag_o14024");

      Assert.IsFalse(new JSValue(false).JSEquals(JSValue.Null), "tag_o15001");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueObject { }), "tag_o15002");
      Assert.IsTrue(new JSValue(false).JSEquals(new JSValueArray { }), "tag_o15003");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueArray { "Hello" }), "tag_o15004");
      Assert.IsTrue(new JSValue(false).JSEquals(new JSValueArray { 0 }), "tag_o15005");
      Assert.IsTrue(new JSValue(false).JSEquals(new JSValueArray { "0" }), "tag_o15006");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueArray { 1 }), "tag_o15007");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueArray { "1" }), "tag_o15008");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueArray { true }), "tag_o15009");
      Assert.IsFalse(new JSValue(false).JSEquals(new JSValueArray { "true" }), "tag_o15010");
      Assert.IsTrue(new JSValue(false).JSEquals(""), "tag_o15011");
      Assert.IsTrue(new JSValue(false).JSEquals("0"), "tag_o15012");
      Assert.IsFalse(new JSValue(false).JSEquals("1"), "tag_o15013");
      Assert.IsFalse(new JSValue(false).JSEquals("true"), "tag_o15014");
      Assert.IsFalse(new JSValue(false).JSEquals("false"), "tag_o15015");
      Assert.IsFalse(new JSValue(false).JSEquals("Hello"), "tag_o15016");
      Assert.IsTrue(new JSValue(false).JSEquals(false), "tag_o15017");
      Assert.IsFalse(new JSValue(false).JSEquals(true), "tag_o15018");
      Assert.IsTrue(new JSValue(false).JSEquals(0), "tag_o15019");
      Assert.IsFalse(new JSValue(false).JSEquals(5), "tag_o15020");
      Assert.IsFalse(new JSValue(false).JSEquals(1), "tag_o15021");
      Assert.IsTrue(new JSValue(false).JSEquals(0.0), "tag_o15022");
      Assert.IsFalse(new JSValue(false).JSEquals(0.5), "tag_o15023");
      Assert.IsFalse(new JSValue(false).JSEquals(1.0), "tag_o15024");

      Assert.IsFalse(new JSValue(0).JSEquals(JSValue.Null), "tag_o16001");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueObject { }), "tag_o16002");
      Assert.IsTrue(new JSValue(0).JSEquals(new JSValueArray { }), "tag_o16003");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueArray { "Hello" }), "tag_o16004");
      Assert.IsTrue(new JSValue(0).JSEquals(new JSValueArray { 0 }), "tag_o16005");
      Assert.IsTrue(new JSValue(0).JSEquals(new JSValueArray { "0" }), "tag_o16006");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueArray { 1 }), "tag_o16007");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueArray { "1" }), "tag_o16008");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueArray { true }), "tag_o16009");
      Assert.IsFalse(new JSValue(0).JSEquals(new JSValueArray { "true" }), "tag_o16010");
      Assert.IsTrue(new JSValue(0).JSEquals(""), "tag_o16011");
      Assert.IsTrue(new JSValue(0).JSEquals("0"), "tag_o16012");
      Assert.IsFalse(new JSValue(0).JSEquals("1"), "tag_o16013");
      Assert.IsFalse(new JSValue(0).JSEquals("true"), "tag_o16014");
      Assert.IsFalse(new JSValue(0).JSEquals("false"), "tag_o16015");
      Assert.IsFalse(new JSValue(0).JSEquals("Hello"), "tag_o16016");
      Assert.IsTrue(new JSValue(0).JSEquals(false), "tag_o16017");
      Assert.IsFalse(new JSValue(0).JSEquals(true), "tag_o16018");
      Assert.IsTrue(new JSValue(0).JSEquals(0), "tag_o16019");
      Assert.IsFalse(new JSValue(0).JSEquals(5), "tag_o16020");
      Assert.IsFalse(new JSValue(0).JSEquals(1), "tag_o16021");
      Assert.IsTrue(new JSValue(0).JSEquals(0.0), "tag_o16022");
      Assert.IsFalse(new JSValue(0).JSEquals(0.5), "tag_o16023");
      Assert.IsFalse(new JSValue(0).JSEquals(1.0), "tag_o16024");

      Assert.IsFalse(new JSValue(1).JSEquals(JSValue.Null), "tag_o17001");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueObject { }), "tag_o17002");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { }), "tag_o17003");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { "Hello" }), "tag_o17004");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { 0 }), "tag_o17005");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { "0" }), "tag_o17006");
      Assert.IsTrue(new JSValue(1).JSEquals(new JSValueArray { 1 }), "tag_o17007");
      Assert.IsTrue(new JSValue(1).JSEquals(new JSValueArray { "1" }), "tag_o17008");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { true }), "tag_o17009");
      Assert.IsFalse(new JSValue(1).JSEquals(new JSValueArray { "true" }), "tag_o17010");
      Assert.IsFalse(new JSValue(1).JSEquals(""), "tag_o17011");
      Assert.IsFalse(new JSValue(1).JSEquals("0"), "tag_o17012");
      Assert.IsTrue(new JSValue(1).JSEquals("1"), "tag_o17013");
      Assert.IsFalse(new JSValue(1).JSEquals("true"), "tag_o17014");
      Assert.IsFalse(new JSValue(1).JSEquals("false"), "tag_o17015");
      Assert.IsFalse(new JSValue(1).JSEquals("Hello"), "tag_o17016");
      Assert.IsFalse(new JSValue(1).JSEquals(false), "tag_o17017");
      Assert.IsTrue(new JSValue(1).JSEquals(true), "tag_o17018");
      Assert.IsFalse(new JSValue(1).JSEquals(0), "tag_o17019");
      Assert.IsFalse(new JSValue(1).JSEquals(5), "tag_o17020");
      Assert.IsTrue(new JSValue(1).JSEquals(1), "tag_o17021");
      Assert.IsFalse(new JSValue(1).JSEquals(0.0), "tag_o17022");
      Assert.IsFalse(new JSValue(1).JSEquals(0.5), "tag_o17023");
      Assert.IsTrue(new JSValue(1).JSEquals(1.0), "tag_o17024");

      Assert.IsFalse(new JSValue(5).JSEquals(JSValue.Null), "tag_o18001");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueObject { }), "tag_o18002");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { }), "tag_o18003");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { "Hello" }), "tag_o18004");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { 0 }), "tag_o18005");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { "0" }), "tag_o18006");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { 1 }), "tag_o18007");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { "1" }), "tag_o18008");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { true }), "tag_o18009");
      Assert.IsFalse(new JSValue(5).JSEquals(new JSValueArray { "true" }), "tag_o18010");
      Assert.IsFalse(new JSValue(5).JSEquals(""), "tag_o18011");
      Assert.IsFalse(new JSValue(5).JSEquals("0"), "tag_o18012");
      Assert.IsFalse(new JSValue(5).JSEquals("1"), "tag_o18013");
      Assert.IsFalse(new JSValue(5).JSEquals("true"), "tag_o18014");
      Assert.IsFalse(new JSValue(5).JSEquals("false"), "tag_o18015");
      Assert.IsFalse(new JSValue(5).JSEquals("Hello"), "tag_o18016");
      Assert.IsFalse(new JSValue(5).JSEquals(false), "tag_o18017");
      Assert.IsFalse(new JSValue(5).JSEquals(true), "tag_o18018");
      Assert.IsFalse(new JSValue(5).JSEquals(0), "tag_o18019");
      Assert.IsTrue(new JSValue(5).JSEquals(5), "tag_o18020");
      Assert.IsFalse(new JSValue(5).JSEquals(1), "tag_o18021");
      Assert.IsFalse(new JSValue(5).JSEquals(0.0), "tag_o18022");
      Assert.IsFalse(new JSValue(5).JSEquals(0.5), "tag_o18023");
      Assert.IsFalse(new JSValue(5).JSEquals(1.0), "tag_o18024");

      Assert.IsFalse(new JSValue(0.0).JSEquals(JSValue.Null), "tag_o19001");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueObject { }), "tag_o19002");
      Assert.IsTrue(new JSValue(0.0).JSEquals(new JSValueArray { }), "tag_o19003");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueArray { "Hello" }), "tag_o19004");
      Assert.IsTrue(new JSValue(0.0).JSEquals(new JSValueArray { 0 }), "tag_o19005");
      Assert.IsTrue(new JSValue(0.0).JSEquals(new JSValueArray { "0" }), "tag_o19006");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueArray { 1 }), "tag_o19007");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueArray { "1" }), "tag_o19008");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueArray { true }), "tag_o19009");
      Assert.IsFalse(new JSValue(0.0).JSEquals(new JSValueArray { "true" }), "tag_o19010");
      Assert.IsTrue(new JSValue(0.0).JSEquals(""), "tag_o19011");
      Assert.IsTrue(new JSValue(0.0).JSEquals("0"), "tag_o19012");
      Assert.IsFalse(new JSValue(0.0).JSEquals("1"), "tag_o19013");
      Assert.IsFalse(new JSValue(0.0).JSEquals("true"), "tag_o19014");
      Assert.IsFalse(new JSValue(0.0).JSEquals("false"), "tag_o19015");
      Assert.IsFalse(new JSValue(0.0).JSEquals("Hello"), "tag_o19016");
      Assert.IsTrue(new JSValue(0.0).JSEquals(false), "tag_o19017");
      Assert.IsFalse(new JSValue(0.0).JSEquals(true), "tag_o19018");
      Assert.IsTrue(new JSValue(0.0).JSEquals(0), "tag_o19019");
      Assert.IsFalse(new JSValue(0.0).JSEquals(5), "tag_o19020");
      Assert.IsFalse(new JSValue(0.0).JSEquals(1), "tag_o19021");
      Assert.IsTrue(new JSValue(0.0).JSEquals(0.0), "tag_o19022");
      Assert.IsFalse(new JSValue(0.0).JSEquals(0.5), "tag_o19023");
      Assert.IsFalse(new JSValue(0.0).JSEquals(1.0), "tag_o19024");

      Assert.IsFalse(new JSValue(1.0).JSEquals(JSValue.Null), "tag_o20001");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueObject { }), "tag_o20002");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { }), "tag_o20003");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { "Hello" }), "tag_o20004");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { 0 }), "tag_o20005");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { "0" }), "tag_o20006");
      Assert.IsTrue(new JSValue(1.0).JSEquals(new JSValueArray { 1 }), "tag_o20007");
      Assert.IsTrue(new JSValue(1.0).JSEquals(new JSValueArray { "1" }), "tag_o20008");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { true }), "tag_o20009");
      Assert.IsFalse(new JSValue(1.0).JSEquals(new JSValueArray { "true" }), "tag_o20010");
      Assert.IsFalse(new JSValue(1.0).JSEquals(""), "tag_o20011");
      Assert.IsFalse(new JSValue(1.0).JSEquals("0"), "tag_o20012");
      Assert.IsTrue(new JSValue(1.0).JSEquals("1"), "tag_o20013");
      Assert.IsFalse(new JSValue(1.0).JSEquals("true"), "tag_o20014");
      Assert.IsFalse(new JSValue(1.0).JSEquals("false"), "tag_o20015");
      Assert.IsFalse(new JSValue(1.0).JSEquals("Hello"), "tag_o20016");
      Assert.IsFalse(new JSValue(1.0).JSEquals(false), "tag_o20017");
      Assert.IsTrue(new JSValue(1.0).JSEquals(true), "tag_o20018");
      Assert.IsFalse(new JSValue(1.0).JSEquals(0), "tag_o20019");
      Assert.IsFalse(new JSValue(1.0).JSEquals(5), "tag_o20020");
      Assert.IsTrue(new JSValue(1.0).JSEquals(1), "tag_o20021");
      Assert.IsFalse(new JSValue(1.0).JSEquals(0.0), "tag_o20022");
      Assert.IsFalse(new JSValue(1.0).JSEquals(0.5), "tag_o20023");
      Assert.IsTrue(new JSValue(1.0).JSEquals(1.0), "tag_o20024");

      Assert.IsFalse(new JSValue(0.5).JSEquals(JSValue.Null), "tag_o21001");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueObject { }), "tag_o21002");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { }), "tag_o21003");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { "Hello" }), "tag_o21004");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { 0 }), "tag_o21005");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { "0" }), "tag_o21006");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { 1 }), "tag_o21007");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { "1" }), "tag_o21008");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { true }), "tag_o21009");
      Assert.IsFalse(new JSValue(0.5).JSEquals(new JSValueArray { "true" }), "tag_o21010");
      Assert.IsFalse(new JSValue(0.5).JSEquals(""), "tag_o21011");
      Assert.IsFalse(new JSValue(0.5).JSEquals("0"), "tag_o21012");
      Assert.IsFalse(new JSValue(0.5).JSEquals("1"), "tag_o21013");
      Assert.IsFalse(new JSValue(0.5).JSEquals("true"), "tag_o21014");
      Assert.IsFalse(new JSValue(0.5).JSEquals("false"), "tag_o21015");
      Assert.IsFalse(new JSValue(0.5).JSEquals("Hello"), "tag_o21016");
      Assert.IsFalse(new JSValue(0.5).JSEquals(false), "tag_o21017");
      Assert.IsFalse(new JSValue(0.5).JSEquals(true), "tag_o21018");
      Assert.IsFalse(new JSValue(0.5).JSEquals(0), "tag_o21019");
      Assert.IsFalse(new JSValue(0.5).JSEquals(5), "tag_o21020");
      Assert.IsFalse(new JSValue(0.5).JSEquals(1), "tag_o21021");
      Assert.IsFalse(new JSValue(0.5).JSEquals(0.0), "tag_o21022");
      Assert.IsTrue(new JSValue(0.5).JSEquals(0.5), "tag_o21023");
      Assert.IsFalse(new JSValue(0.5).JSEquals(1.0), "tag_o21024");
    }

    [TestMethod]
    public void TestEquals()
    {
      void CheckEquals(JSValue left, JSValue right, string tag)
      {
        Assert.IsTrue(left.Equals(right), "Equals: {0}", tag);
        Assert.IsTrue(left.Equals((object)right), "Equals(object): {0}", tag);
        Assert.IsTrue(left == right, "operator ==: {0}", tag);
      }

      void CheckNotEquals(JSValue left, JSValue right, string tag)
      {
        Assert.IsFalse(left.Equals(right), "!Equals: {0}", tag);
        Assert.IsFalse(left.Equals((object)right), "!Equals(object): {0}", tag);
        Assert.IsTrue(left != right, "operator !=: {0}", tag);
      }

      CheckEquals(new JSValueObject { }, new JSValueObject { }, "tag_p101");
      CheckEquals(new JSValueObject { ["prop1"] = 1 }, new JSValueObject { ["prop1"] = 1 }, "tag_p102");
      CheckEquals(new JSValueObject { ["prop1"] = 1, ["prop2"] = "Hello" },
                  new JSValueObject { ["prop1"] = 1, ["prop2"] = "Hello" }, "tag_p103");
      CheckEquals(new JSValueObject { ["prop1"] = new JSValueObject { } },
                  new JSValueObject { ["prop1"] = new JSValueObject { } }, "tag_p104");
      CheckEquals(new JSValueObject { ["prop1"] = new JSValueObject { ["prop1"] = 1 } },
                  new JSValueObject { ["prop1"] = new JSValueObject { ["prop1"] = 1 } }, "tag_p105");
      CheckEquals(new JSValueObject { ["prop1"] = new JSValueArray { } },
                  new JSValueObject { ["prop1"] = new JSValueArray { } }, "tag_p106");
      CheckEquals(new JSValueObject { ["prop1"] = new JSValueArray { 1 } },
                  new JSValueObject { ["prop1"] = new JSValueArray { 1 } }, "tag_p107");
      CheckNotEquals(new JSValueObject { ["prop1"] = 1 }, new JSValueObject { }, "tag_p108");
      CheckNotEquals(new JSValueObject { ["prop1"] = 1 }, new JSValueObject { ["prop1"] = 2 }, "tag_p109");
      CheckNotEquals(new JSValueObject { }, new JSValueArray { }, "tag_p110");
      CheckNotEquals(new JSValueObject { }, "", "tag_p111");
      CheckNotEquals(new JSValueObject { }, false, "tag_p112");
      CheckNotEquals(new JSValueObject { }, true, "tag_p113");
      CheckNotEquals(new JSValueObject { }, 0, "tag_p114");
      CheckNotEquals(new JSValueObject { }, 0.0, "tag_p115");


      CheckEquals(new JSValueArray { }, new JSValueArray { }, "tag_p201");
      CheckEquals(new JSValueArray { 1 }, new JSValueArray { 1 }, "tag_p202");
      CheckEquals(new JSValueArray { 1, "Hello" },
                  new JSValueArray { 1, "Hello" }, "tag_p203");
      CheckEquals(new JSValueArray { new JSValueArray { } },
                  new JSValueArray { new JSValueArray { } }, "tag_p204");
      CheckEquals(new JSValueArray { new JSValueArray { 1 } },
                  new JSValueArray { new JSValueArray { 1 } }, "tag_p205");
      CheckEquals(new JSValueArray { new JSValueObject { } },
                  new JSValueArray { new JSValueObject { } }, "tag_p206");
      CheckEquals(new JSValueArray { new JSValueObject { ["prop1"] = 1 } },
                  new JSValueArray { new JSValueObject { ["prop1"] = 1 } }, "tag_p207");
      CheckNotEquals(new JSValueArray { 1 }, new JSValueArray { }, "tag_p208");
      CheckNotEquals(new JSValueArray { 1 }, new JSValueArray { 2 }, "tag_p209");
      CheckNotEquals(new JSValueArray { }, new JSValueObject { }, "tag_p210");
      CheckNotEquals(new JSValueArray { }, "", "tag_p211");
      CheckNotEquals(new JSValueArray { }, false, "tag_p212");
      CheckNotEquals(new JSValueArray { }, true, "tag_p213");
      CheckNotEquals(new JSValueArray { }, 0, "tag_p214");
      CheckNotEquals(new JSValueArray { }, 0.0, "tag_p215");

      CheckEquals("", "", "tag_p301");
      CheckEquals("Hello", "Hello", "tag_p302");
      CheckNotEquals("Hello1", "Hello2", "tag_p303");
      CheckNotEquals("", new JSValueObject { }, "tag_p304");
      CheckNotEquals("", new JSValueArray { }, "tag_p305");
      CheckNotEquals("", false, "tag_p306");
      CheckNotEquals("", 0, "tag_p307");
      CheckNotEquals("", 0.0, "tag_p308");

      CheckEquals(false, false, "tag_p401");
      CheckEquals(true, true, "tag_p402");
      CheckNotEquals(false, true, "tag_p403");
      CheckNotEquals(true, false, "tag_p404");
      CheckNotEquals(false, new JSValueObject { }, "tag_p405");
      CheckNotEquals(false, new JSValueArray { }, "tag_p406");
      CheckNotEquals(false, "", "tag_p407");
      CheckNotEquals(false, 0, "tag_p408");
      CheckNotEquals(false, 0.0, "tag_p409");

      CheckEquals(0, 0, "tag_p501");
      CheckEquals(42, 42, "tag_p502");
      CheckNotEquals(2, 3, "tag_p503");
      CheckNotEquals(-1, 1, "tag_p504");
      CheckNotEquals(0, new JSValueObject { }, "tag_p505");
      CheckNotEquals(0, new JSValueArray { }, "tag_p506");
      CheckNotEquals(0, "", "tag_p507");
      CheckNotEquals(0, false, "tag_p508");
      CheckNotEquals(0, 0.0, "tag_p509");

      CheckEquals(0.0, 0.0, "tag_p601");
      CheckEquals(4.2, 4.2, "tag_p602");
      CheckNotEquals(0.2, 0.3, "tag_p603");
      CheckNotEquals(-0.1, 0.1, "tag_p604");
      CheckNotEquals(0.0, new JSValueObject { }, "tag_p605");
      CheckNotEquals(0.0, new JSValueArray { }, "tag_p606");
      CheckNotEquals(0.0, "", "tag_p607");
      CheckNotEquals(0.0, false, "tag_p608");
      CheckNotEquals(0.0, 0, "tag_p609");
    }
  }
}
