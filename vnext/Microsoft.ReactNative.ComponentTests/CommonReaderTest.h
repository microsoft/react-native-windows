// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::ReaderTestCases {

// a construction to assert JSValueType::Object without hard-coded property order

inline void TestCheckObject(const std::map<hstring, std::function<void()>> &props, IJSValueReader &reader) {
  std::set<hstring> propNames;
  hstring propertyName;

  for (size_t i = 0; i < props.size(); i++) {
    // ensure that we succeeded in reading a property name
    TestCheck(reader.GetNextObjectProperty(propertyName));

    // ensure that we don't get duplicated property names from the reader
    TestCheckEqual(propNames.end(), propNames.find(propertyName));
    propNames.insert(propertyName);

    // ensure that the property value is expected
    auto it = props.find(propertyName);
    TestCheck(it != props.end());
    it->second();
  }

  // ensure that we read all properties as expected
  TestCheck(!reader.GetNextObjectProperty(propertyName));
  TestCheckEqual(props.size(), propNames.size());
}

// null
struct PrimitiveNull {
  static void Write(IJSValueWriter &writer) {
    writer.WriteNull();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Null, reader.ValueType());
  }
};

// false
struct PrimitiveBoolean {
  static void Write(IJSValueWriter &writer) {
    writer.WriteBoolean(false);
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheck(!reader.GetBoolean());
  }
};

// 12345
struct PrimitiveInt {
  static void Write(IJSValueWriter &writer) {
    writer.WriteInt64(12345);
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Int64, reader.ValueType());
    TestCheckEqual(12345, reader.GetInt64());
  }
};

// 0.5
struct PrimitiveDouble {
  static void Write(IJSValueWriter &writer) {
    writer.WriteDouble(0.5);
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Double, reader.ValueType());
    TestCheckEqual(0.5, reader.GetDouble());
  }
};

// "This is a string"
struct PrimitiveString {
  static void Write(IJSValueWriter &writer) {
    writer.WriteString(L"This is a string");
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"This is a string", reader.GetString());
  }
};

// []
struct EmptyArray {
  static void Write(IJSValueWriter &writer) {
    writer.WriteArrayBegin();
    writer.WriteArrayEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(!reader.GetNextArrayItem());
  }
};

// [true]
struct ArrayWitnOneElement {
  static void Write(IJSValueWriter &writer) {
    writer.WriteArrayBegin();
    writer.WriteBoolean(true);
    writer.WriteArrayEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheck(reader.GetBoolean());
    TestCheck(!reader.GetNextArrayItem());
  }
};

// [0, 1, 2, 3, 4]
struct ArrayWitnMultipleElement {
  static void Write(IJSValueWriter &writer) {
    writer.WriteArrayBegin();
    for (int i = 0; i < 5; i++) {
      writer.WriteInt64(i);
    }
    writer.WriteArrayEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    for (int i = 0; i < 5; i++) {
      TestCheck(reader.GetNextArrayItem());
      TestCheckEqual(JSValueType::Int64, reader.ValueType());
      TestCheckEqual(i, reader.GetInt64());
    }
    TestCheck(!reader.GetNextArrayItem());
  }
};

// [[[]]]
struct EmptyNestedArray {
  static void Write(IJSValueWriter &writer) {
    writer.WriteArrayBegin();
    writer.WriteArrayBegin();
    writer.WriteArrayBegin();
    writer.WriteArrayEnd();
    writer.WriteArrayEnd();
    writer.WriteArrayEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(!reader.GetNextArrayItem());
    TestCheck(!reader.GetNextArrayItem());
    TestCheck(!reader.GetNextArrayItem());
  }
};

// [[null], [true], [12345]]
struct NestedArrayWithPrimitiveValues {
  static void Write(IJSValueWriter &writer) {
    writer.WriteArrayBegin();

    writer.WriteArrayBegin();
    writer.WriteNull();
    writer.WriteArrayEnd();

    writer.WriteArrayBegin();
    writer.WriteBoolean(true);
    writer.WriteArrayEnd();

    writer.WriteArrayBegin();
    writer.WriteInt64(12345);
    writer.WriteArrayEnd();

    writer.WriteArrayEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Array, reader.ValueType());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Null, reader.ValueType());
    TestCheck(!reader.GetNextArrayItem());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheck(reader.GetBoolean());
    TestCheck(!reader.GetNextArrayItem());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Int64, reader.ValueType());
    TestCheckEqual(12345, reader.GetInt64());
    TestCheck(!reader.GetNextArrayItem());

    TestCheck(!reader.GetNextArrayItem());
  }
};

// {}
struct EmptyObject {
  static void Write(IJSValueWriter &writer) {
    writer.WriteObjectBegin();
    writer.WriteObjectEnd();
  }

  static void Read(IJSValueReader &reader) {
    hstring propertyName;
    TestCheckEqual(JSValueType::Object, reader.ValueType());
    TestCheck(!reader.GetNextObjectProperty(propertyName));
  }
};

// {a:null, b:true, c:12345}
struct SimpleObject {
  static void Write(IJSValueWriter &writer) {
    writer.WriteObjectBegin();
    writer.WritePropertyName(L"a");
    writer.WriteNull();
    writer.WritePropertyName(L"b");
    writer.WriteBoolean(true);
    writer.WritePropertyName(L"c");
    writer.WriteInt64(12345);
    writer.WriteObjectEnd();
  }

  static void Read(IJSValueReader &reader) {
    TestCheckEqual(JSValueType::Object, reader.ValueType());
    TestCheckObject(
        {{L"a", [&]() { TestCheckEqual(JSValueType::Null, reader.ValueType()); }},
         {L"b",
          [&]() {
            TestCheckEqual(JSValueType::Boolean, reader.ValueType());
            TestCheck(reader.GetBoolean());
          }},
         {L"c",
          [&]() {
            TestCheckEqual(JSValueType::Int64, reader.ValueType());
            TestCheckEqual(12345, reader.GetInt64());
          }}},
        reader);
  }
};

// {a:{x:1}, b:{x:2}, c:{x:3} ,d:{x:4}, e:{x:5}}
struct NestedObjectWithPrimitiveValues {
  static void Write(IJSValueWriter &writer) {
    hstring props[] = {L"a", L"b", L"c", L"d", L"e"};
    int index = 0;

    writer.WriteObjectBegin();
    for (auto prop : props) {
      writer.WritePropertyName(prop);
      writer.WriteObjectBegin();
      writer.WritePropertyName(L"x");
      writer.WriteInt64(++index);
      writer.WriteObjectEnd();
    }
    writer.WriteObjectEnd();
  }

  static void Read(IJSValueReader &reader) {
    hstring props[] = {L"a", L"b", L"c", L"d", L"e"};
    int index = 0;

    hstring propertyName;
    std::map<hstring, std::function<void()>> assertions;
    for (auto prop : props) {
      ++index;
      assertions.insert({prop, [&, index]() {
                           TestCheckEqual(JSValueType::Object, reader.ValueType());
                           TestCheck(reader.GetNextObjectProperty(propertyName));
                           TestCheckEqual(L"x", propertyName);
                           TestCheckEqual(JSValueType::Int64, reader.ValueType());
                           TestCheckEqual(index, reader.GetInt64());
                           TestCheck(!reader.GetNextObjectProperty(propertyName));
                         }});
    }

    TestCheckEqual(JSValueType::Object, reader.ValueType());
    TestCheckObject(assertions, reader);
  }
};

//

} // namespace winrt::Microsoft::ReactNative::ReaderTestCases

// IMPORT_READER_TEST_CASES macro runs the same set of test cases
// on DynamicReader+DynamicWriter and JsiReader+JsiWriter
// in this way we ensure that two implementation behaves exactly the same

#define IMPORT_READER_TEST_CASE(name)       \
  TEST_METHOD(name) {                       \
    RunReaderTest<ReaderTestCases::name>(); \
  }

#define IMPORT_ARGUMENT_READER_TEST_CASES           \
  IMPORT_READER_TEST_CASE(EmptyArray)               \
  IMPORT_READER_TEST_CASE(ArrayWitnOneElement)      \
  IMPORT_READER_TEST_CASE(ArrayWitnMultipleElement) \
  IMPORT_READER_TEST_CASE(EmptyNestedArray)         \
  IMPORT_READER_TEST_CASE(NestedArrayWithPrimitiveValues)

#define IMPORT_READER_TEST_CASES            \
  IMPORT_READER_TEST_CASE(PrimitiveNull)    \
  IMPORT_READER_TEST_CASE(PrimitiveBoolean) \
  IMPORT_READER_TEST_CASE(PrimitiveInt)     \
  IMPORT_READER_TEST_CASE(PrimitiveDouble)  \
  IMPORT_READER_TEST_CASE(PrimitiveString)  \
  IMPORT_ARGUMENT_READER_TEST_CASES         \
  IMPORT_READER_TEST_CASE(EmptyObject)      \
  IMPORT_READER_TEST_CASE(SimpleObject)     \
  IMPORT_READER_TEST_CASE(NestedObjectWithPrimitiveValues)
