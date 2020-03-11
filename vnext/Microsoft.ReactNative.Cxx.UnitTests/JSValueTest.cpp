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

    TestCheck(jsValue["NullValue"].Type() == JSValueType::Null);
    TestCheck(jsValue["ObjValue"].Type() == JSValueType::Object);
    TestCheck(jsValue["ArrayValue"].Type() == JSValueType::Array);
    TestCheck(jsValue["StringValue"].Type() == JSValueType::String);
    TestCheck(jsValue["BoolValue"].Type() == JSValueType::Boolean);
    TestCheck(jsValue["IntValue"].Type() == JSValueType::Int64);
    TestCheck(jsValue["DoubleValue"].Type() == JSValueType::Double);

    TestCheck(jsValue["NullValue"].IsNull());
    TestCheck(jsValue["ObjValue"].TryGetObject()->empty());
    TestCheck(jsValue["ArrayValue"].TryGetArray()->empty());
    TestCheck(jsValue["StringValue"] == "Hello");
    TestCheck(jsValue["BoolValue"] == true);
    TestCheck(jsValue["IntValue"] == 42);
    TestCheck(jsValue["DoubleValue"] == 4.5);
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
    TestCheck(jsValue["NestedObj"].Type() == JSValueType::Object);
    auto const &nestedObj = *jsValue["NestedObj"].TryGetObject();

    TestCheck(nestedObj["NullValue"].Type() == JSValueType::Null);
    TestCheck(nestedObj["ObjValue"].Type() == JSValueType::Object);
    TestCheck(nestedObj["ArrayValue"].Type() == JSValueType::Array);
    TestCheck(nestedObj["StringValue"].Type() == JSValueType::String);
    TestCheck(nestedObj["BoolValue"].Type() == JSValueType::Boolean);
    TestCheck(nestedObj["IntValue"].Type() == JSValueType::Int64);
    TestCheck(nestedObj["DoubleValue"].Type() == JSValueType::Double);

    TestCheck(nestedObj["NullValue"].IsNull());
    TestCheck(nestedObj["ObjValue"].TryGetObject()->empty());
    TestCheck(nestedObj["ArrayValue"].TryGetArray()->empty());
    TestCheck(nestedObj["StringValue"] == "Hello");
    TestCheck(nestedObj["BoolValue"] == true);
    TestCheck(nestedObj["IntValue"] == 42);
    TestCheck(nestedObj["DoubleValue"] == 4.5);
  }

  TEST_METHOD(TestReadArray) {
    const wchar_t *json = LR"JSON([null, {}, [], "Hello", true, 42, 4.5])JSON";
    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue.Type() == JSValueType::Array);

    TestCheck(jsValue[0].Type() == JSValueType::Null);
    TestCheck(jsValue[1].Type() == JSValueType::Object);
    TestCheck(jsValue[2].Type() == JSValueType::Array);
    TestCheck(jsValue[3].Type() == JSValueType::String);
    TestCheck(jsValue[4].Type() == JSValueType::Boolean);
    TestCheck(jsValue[5].Type() == JSValueType::Int64);
    TestCheck(jsValue[6].Type() == JSValueType::Double);

    TestCheck(jsValue[0].IsNull());
    TestCheck(jsValue[1].TryGetObject()->empty());
    TestCheck(jsValue[2].TryGetArray()->empty());
    TestCheck(jsValue[3] == "Hello");
    TestCheck(jsValue[4] == true);
    TestCheck(jsValue[5] == 42);
    TestCheck(jsValue[6] == 4.5);
  }

  TEST_METHOD(TestReadNestedArray) {
    const wchar_t *json = LR"JSON([[null, {}, [], "Hello", true, 42, 4.5]])JSON";
    IJSValueReader reader = make<JsonJSValueReader>(json);

    JSValue jsValue = JSValue::ReadFrom(reader);
    TestCheck(jsValue[0].TryGetArray());
    auto const &nestedArr = *jsValue[0].TryGetArray();

    TestCheck(nestedArr[0].Type() == JSValueType::Null);
    TestCheck(nestedArr[1].Type() == JSValueType::Object);
    TestCheck(nestedArr[2].Type() == JSValueType::Array);
    TestCheck(nestedArr[3].Type() == JSValueType::String);
    TestCheck(nestedArr[4].Type() == JSValueType::Boolean);
    TestCheck(nestedArr[5].Type() == JSValueType::Int64);
    TestCheck(nestedArr[6].Type() == JSValueType::Double);

    TestCheck(nestedArr[0].IsNull());
    TestCheck(nestedArr[1].TryGetObject()->empty());
    TestCheck(nestedArr[2].TryGetArray()->empty());
    TestCheck(nestedArr[3] == "Hello");
    TestCheck(nestedArr[4] == true);
    TestCheck(nestedArr[5] == 42);
    TestCheck(nestedArr[6] == 4.5);
  }

  TEST_METHOD(TestJSValueObject1) {
    JSValue value = JSValueObject{{"prop1", 1}, {"prop2", "Two"}};
    TestCheck(value.Type() == JSValueType::Object);
    TestCheck(value.PropertyCount() == 2);
    TestCheck(value["prop1"] == 1);
    TestCheck(value["prop2"] == "Two");
    TestCheck(value.AsObject()["prop1"] == 1);
    TestCheck(value.AsObject()["prop2"] == "Two");
  }

  TEST_METHOD(TestJSValueObject2) {
    JSValueObject obj;
    obj["prop1"] = 1;
    obj["prop2"] = "Two";
    JSValue value = std::move(obj);
    TestCheck(value.Type() == JSValueType::Object);
    TestCheck(value.PropertyCount() == 2);
    TestCheck(value["prop1"] == 1);
    TestCheck(value["prop2"] == "Two");
    TestCheck(value.AsObject()["prop1"] == 1);
    TestCheck(value.AsObject()["prop2"] == "Two");
  }

  TEST_METHOD(TestJSValueArray1) {
    JSValue value = JSValueArray{1, "Two", 3.3, true, nullptr};
    TestCheck(value.Type() == JSValueType::Array);
    TestCheck(value.ItemCount() == 5);
    TestCheck(value[0] == 1);
    TestCheck(value[1] == "Two");
    TestCheck(value[2] == 3.3);
    TestCheck(value[3] == true);
    TestCheck(value[4] == nullptr);
  }

  TEST_METHOD(TestJSValueArray2) {
    JSValueArray arr(5);
    arr[0] = 1;
    arr[1] = "Two";
    arr[2] = 3.3;
    arr[3] = true;
    arr[4] = nullptr;
    JSValue value = std::move(arr);
    TestCheck(value.Type() == JSValueType::Array);
    TestCheck(value.ItemCount() == 5);
    TestCheck(value[0] == 1);
    TestCheck(value[1] == "Two");
    TestCheck(value[2] == 3.3);
    TestCheck(value[3] == true);
    TestCheck(value[4] == nullptr);
  }

  void CheckConversion(
      JSValue const &value, char const *strVal, bool boolVal, int64_t intVal, double doubleVal) noexcept {
    TestCheckEqual(strVal, value.AsString());
    TestCheckEqual(boolVal, value.AsBoolean());
    TestCheckEqual(intVal, value.AsInt64());
    if (std::isnan(doubleVal)) {
      TestCheck(std::isnan(value.AsDouble()));
    } else {
      TestCheckEqual(doubleVal, value.AsDouble());
    }
  }

  TEST_METHOD(TestDefaultAsConverters) {
    CheckConversion(false, "false", false, 0, 0);
    CheckConversion(true, "true", true, 1, 1);
    CheckConversion(0, "0", false, 0, 0);
    CheckConversion(1, "1", true, 1, 1);
    CheckConversion("0", "0", true, 0, 0);
    CheckConversion("000", "000", true, 0, 0);
    CheckConversion("1", "1", true, 1, 1);
    CheckConversion(NAN, "NaN", false, 0, NAN);
    CheckConversion(INFINITY, "Infinity", true, std::numeric_limits<int64_t>::max(), INFINITY);
    CheckConversion(-INFINITY, "-Infinity", true, std::numeric_limits<int64_t>::min(), -INFINITY);
    CheckConversion("", "", false, 0, 0);
    CheckConversion("20", "20", true, 20, 20);
    CheckConversion("twenty", "twenty", true, 0, NAN);
    CheckConversion(JSValueArray{}, "", true, 0, 0);
    CheckConversion(JSValueArray{20}, "20", true, 20, 20);
    CheckConversion(JSValueArray{10, 20}, "10,20", true, 0, NAN);
    CheckConversion(JSValueArray{"twenty"}, "twenty", true, 0, NAN);
    CheckConversion(JSValueArray{"ten", "twenty"}, "ten,twenty", true, 0, NAN);
    CheckConversion(JSValueArray{NAN}, "NaN", true, 0, NAN);
    CheckConversion(JSValueObject{}, "[object Object]", true, 0, NAN);
    CheckConversion(nullptr, "null", false, 0, 0);
  }

  TEST_METHOD(TestEqualsWithConversion) {
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(nullptr) == true);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{nullptr}.EqualsAfterConversion(0.0) == false);

    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueObject{}) == true);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion("[object Object]") == true);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{JSValueObject{}}.EqualsAfterConversion(1.0) == false);

    TestCheck(
        JSValue(JSValueObject{{"prop1", 2}, {"prop2", false}})
            .EqualsAfterConversion(JSValueObject{{"prop2", 0}, {"prop1", "2"}}) == true);
    TestCheck(
        JSValue(JSValueObject{{"prop1", 2}, {"prop2", false}}).EqualsAfterConversion(JSValueObject{{"prop2", 0}}) ==
        false);
    TestCheck(
        JSValue(JSValueObject{{"prop1", 2}, {"prop2", false}})
            .EqualsAfterConversion(JSValueObject{{"prop1", 2}, {"prop25", false}}) == false);
    TestCheck(
        JSValue(JSValueObject{{"prop1", 2}, {"prop2", false}})
            .EqualsAfterConversion(JSValueObject{{"prop1", 2}, {"prop2", true}}) == false);

    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{}) == true);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("") == true);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{JSValueArray{}}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{1}) == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{"1"}) == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{true}) == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("1") == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(true) == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(1) == true);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{JSValueArray{1}}.EqualsAfterConversion(1.0) == true);

    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{"Hello"}) == true);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion("Hello") == true);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{JSValueArray{"Hello"}}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{0, 1}) == true);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{false, true}) == true);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"0", "1"}) == true);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(JSValueArray{"0", true}) == true);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("0") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("1") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("0,1") == true);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("true") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("false") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(false) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(true) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(0) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(5) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(1) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue(JSValueArray{0, 1}).EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{""}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueArray{}) == true);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(JSValueArray{""}) == true);
    TestCheck(JSValue{""}.EqualsAfterConversion("") == true);
    TestCheck(JSValue{""}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{""}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{""}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{""}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{""}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{""}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(JSValueArray{"Hello"}) == true);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion("Hello") == true);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{"Hello"}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{"0"}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(JSValueArray{"0"}) == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(JSValueArray{0}) == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion("0") == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{"0"}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{"1"}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(JSValueArray{"1"}) == true);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(JSValueArray{1}) == true);
    TestCheck(JSValue{"1"}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion("1") == true);
    TestCheck(JSValue{"1"}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(true) == true);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(1) == true);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{"1"}.EqualsAfterConversion(1.0) == true);

    TestCheck(JSValue{"true"}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueArray{true}) == true);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(JSValueArray{"true"}) == true);
    TestCheck(JSValue{"true"}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion("true") == true);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{"true"}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{"[object Object]"}.EqualsAfterConversion(JSValueObject{}) == true);

    TestCheck(JSValue{true}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{1}) == true);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{"1"}) == true);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{true}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{true}.EqualsAfterConversion("1") == true);
    TestCheck(JSValue{true}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{true}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{true}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(true) == true);
    TestCheck(JSValue{true}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(1) == true);
    TestCheck(JSValue{true}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{true}.EqualsAfterConversion(1.0) == true);

    TestCheck(JSValue{false}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{}) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{0}) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{"0"}) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion("") == true);
    TestCheck(JSValue{false}.EqualsAfterConversion("0") == true);
    TestCheck(JSValue{false}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{false}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{false}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{false}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{false}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{false}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{0}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{}) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{0}) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{"0"}) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion("") == true);
    TestCheck(JSValue{0}.EqualsAfterConversion("0") == true);
    TestCheck(JSValue{0}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{0}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{0}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{0}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{0}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{0}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{1}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{1}) == true);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{"1"}) == true);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{1}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{1}.EqualsAfterConversion("1") == true);
    TestCheck(JSValue{1}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{1}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{1}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(true) == true);
    TestCheck(JSValue{1}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(1) == true);
    TestCheck(JSValue{1}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{1}.EqualsAfterConversion(1.0) == true);

    TestCheck(JSValue{5}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(5) == true);
    TestCheck(JSValue{5}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{5}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{0.0}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{}) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{0}) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{"0"}) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("") == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("0") == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(false) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(0) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(0.0) == true);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{0.0}.EqualsAfterConversion(1.0) == false);

    TestCheck(JSValue{1.0}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{1}) == true);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{"1"}) == true);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("1") == true);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(true) == true);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(1) == true);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(0.5) == false);
    TestCheck(JSValue{1.0}.EqualsAfterConversion(1.0) == true);

    TestCheck(JSValue{0.5}.EqualsAfterConversion(nullptr) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueObject{}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{"Hello"}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{0}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{"0"}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{1}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{"1"}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{true}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(JSValueArray{"true"}) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("0") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("1") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("true") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("false") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion("Hello") == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(false) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(true) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(0) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(5) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(1) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(0.0) == false);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(0.5) == true);
    TestCheck(JSValue{0.5}.EqualsAfterConversion(1.0) == false);
  }
};

} // namespace winrt::Microsoft::ReactNative
