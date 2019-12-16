// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValue.h"
#include "JsonJSValueReader.h"
#include "catch.hpp"

namespace winrt::Microsoft::ReactNative::Bridge {

TEST_CASE("TestReadObject", "JSValueTest") {
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
  REQUIRE(jsValue.Type() == JSValueType::Object);
  REQUIRE(jsValue.Object().at("NullValue").IsNull());
  REQUIRE(jsValue.Object().at("ObjValue").Object().empty());
  REQUIRE(jsValue.Object().at("ArrayValue").Array().empty());
  REQUIRE(jsValue.Object().at("StringValue").String() == "Hello");
  REQUIRE(jsValue.Object().at("BoolValue").Boolean() == true);
  REQUIRE(jsValue.Object().at("IntValue").Int64() == 42);
  REQUIRE(jsValue.Object().at("DoubleValue").Double() == 4.5);
}

TEST_CASE("TestReadNestedObject", "JSValueTest") {
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
  REQUIRE(jsValue.Type() == JSValueType::Object);
  const auto &nestedObj = jsValue.Object().at("NestedObj").Object();
  REQUIRE(nestedObj.at("NullValue").IsNull());
  REQUIRE(nestedObj.at("ObjValue").Object().empty());
  REQUIRE(nestedObj.at("ArrayValue").Array().empty());
  REQUIRE(nestedObj.at("StringValue").String() == "Hello");
  REQUIRE(nestedObj.at("BoolValue").Boolean() == true);
  REQUIRE(nestedObj.at("IntValue").Int64() == 42);
  REQUIRE(nestedObj.at("DoubleValue").Double() == 4.5);
}

TEST_CASE("TestReadArray", "JSValueTest") {
  const wchar_t *json = LR"JSON([null, {}, [], "Hello", true, 42, 4.5])JSON";
  IJSValueReader reader = make<JsonJSValueReader>(json);

  JSValue jsValue = JSValue::ReadFrom(reader);
  REQUIRE(jsValue.Type() == JSValueType::Array);
  REQUIRE(jsValue.Array()[0].IsNull());
  REQUIRE(jsValue.Array()[1].Object().empty());
  REQUIRE(jsValue.Array()[2].Array().empty());
  REQUIRE(jsValue.Array()[3].String() == "Hello");
  REQUIRE(jsValue.Array()[4].Boolean() == true);
  REQUIRE(jsValue.Array()[5].Int64() == 42);
  REQUIRE(jsValue.Array()[6].Double() == 4.5);
}

TEST_CASE("TestReadNestedArray", "JSValueTest") {
  const wchar_t *json = LR"JSON([[null, {}, [], "Hello", true, 42, 4.5]])JSON";
  IJSValueReader reader = make<JsonJSValueReader>(json);

  JSValue jsValue = JSValue::ReadFrom(reader);
  REQUIRE(jsValue.Type() == JSValueType::Array);
  const auto &nestedArr = jsValue.Array()[0].Array();
  REQUIRE(nestedArr[0].IsNull());
  REQUIRE(nestedArr[1].Object().empty());
  REQUIRE(nestedArr[2].Array().empty());
  REQUIRE(nestedArr[3].String() == "Hello");
  REQUIRE(nestedArr[4].Boolean() == true);
  REQUIRE(nestedArr[5].Int64() == 42);
  REQUIRE(nestedArr[6].Double() == 4.5);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
