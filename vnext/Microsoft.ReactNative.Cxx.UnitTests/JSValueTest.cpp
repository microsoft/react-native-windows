// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValue.h"
#include "JsonJSValueReader.h"

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (JSValueTest) {
  TEST_METHOD(TestReadObject) {
    const wchar_t *json =
        LR"JSON({
        "NullValue": null,
        "ObjValue": {},
        "ArrayValue": [],
        "StringValue": "Hello",
        "BoolValue": true,
        "IntValue": 42,
        "DoubleValue": 4.5
      })JSON";

    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue.Type() == JSValueType::Object);
    TestCheck(jsValue.Object().at("NullValue").IsNull());
    TestCheck(jsValue.Object().at("ObjValue").Object().empty());
    TestCheck(jsValue.Object().at("ArrayValue").Array().empty());
    TestCheck(jsValue.Object().at("StringValue").String() == "Hello");
    TestCheck(jsValue.Object().at("BoolValue").Boolean() == true);
    TestCheck(jsValue.Object().at("IntValue").Int64() == 42);
    TestCheck(jsValue.Object().at("DoubleValue").Double() == 4.5);
  } // namespace winrt::Microsoft::ReactNative

  TEST_METHOD(TestReadNestedObject) {
    const wchar_t *json =
        LR"JSON({
        "NestedObj": {
          "NullValue": null,
          "ObjValue": {},
          "ArrayValue": [],
          "StringValue": "Hello",
          "BoolValue": true,
          "IntValue": 42,
          "DoubleValue": 4.5
        }
      })JSON";

    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue.Type() == JSValueType::Object);
    const auto &nestedObj = jsValue.Object().at("NestedObj").Object();
    TestCheck(nestedObj.at("NullValue").IsNull());
    TestCheck(nestedObj.at("ObjValue").Object().empty());
    TestCheck(nestedObj.at("ArrayValue").Array().empty());
    TestCheck(nestedObj.at("StringValue").String() == "Hello");
    TestCheck(nestedObj.at("BoolValue").Boolean() == true);
    TestCheck(nestedObj.at("IntValue").Int64() == 42);
    TestCheck(nestedObj.at("DoubleValue").Double() == 4.5);
  }

  TEST_METHOD(TestReadArray) {
    const wchar_t *json = LR"JSON([null, {}, [], "Hello", true, 42, 4.5])JSON";
    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue.Type() == JSValueType::Array);
    TestCheck(jsValue.Array()[0].IsNull());
    TestCheck(jsValue.Array()[1].Object().empty());
    TestCheck(jsValue.Array()[2].Array().empty());
    TestCheck(jsValue.Array()[3].String() == "Hello");
    TestCheck(jsValue.Array()[4].Boolean() == true);
    TestCheck(jsValue.Array()[5].Int64() == 42);
    TestCheck(jsValue.Array()[6].Double() == 4.5);
  }

  TEST_METHOD(TestReadNestedArray) {
    const wchar_t *json = LR"JSON([[null, {}, [], "Hello", true, 42, 4.5]])JSON";
    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue.Type() == JSValueType::Array);
    const auto &nestedArr = jsValue.Array()[0].Array();
    TestCheck(nestedArr[0].IsNull());
    TestCheck(nestedArr[1].Object().empty());
    TestCheck(nestedArr[2].Array().empty());
    TestCheck(nestedArr[3].String() == "Hello");
    TestCheck(nestedArr[4].Boolean() == true);
    TestCheck(nestedArr[5].Int64() == 42);
    TestCheck(nestedArr[6].Double() == 4.5);
  }
};

} // namespace winrt::Microsoft::ReactNative
