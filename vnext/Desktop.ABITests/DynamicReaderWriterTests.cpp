// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (DynamicReaderWriterTests) {
 public:
  TEST_METHOD(WriteGetBoolean) {
    TestScalar(&IJSValueWriter::WriteBoolean, &IJSValueReader::GetBoolean, JSValueType::Boolean, true);
  }

  TEST_METHOD(WriteGetInt64) {
    TestScalar(&IJSValueWriter::WriteInt64, &IJSValueReader::GetInt64, JSValueType::Int64, static_cast<int64_t>(123));
  }

  TEST_METHOD(WriteGetDouble) {
    TestScalar(&IJSValueWriter::WriteDouble, &IJSValueReader::GetDouble, JSValueType::Double, 3.14);
  }

  TEST_METHOD(WriteGetString) {
    TestScalar<const param::hstring &, hstring>(
        &IJSValueWriter::WriteString, &IJSValueReader::GetString, JSValueType::String, L"abc");
  }

  TEST_METHOD(WriteNull) {
    IJSValueWriter writer = Microsoft::Internal::TestController::CreateDynamicWriter();
    writer.WriteNull();
    IJSValueReader reader = Microsoft::Internal::TestController::CreateDynamicReader(writer);
    TestCheckEqual(JSValueType::Null, reader.ValueType());
  }

  TEST_METHOD(WriteGetArray) {
    IJSValueWriter writer = Microsoft::Internal::TestController::CreateDynamicWriter();
    writer.WriteArrayBegin();
    writer.WriteBoolean(true);
    writer.WriteInt64(123);
    writer.WriteString(L"abc");
    writer.WriteArrayEnd();

    IJSValueReader reader = Microsoft::Internal::TestController::CreateDynamicReader(writer);
    TestCheckEqual(JSValueType::Array, reader.ValueType());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheckEqual(true, reader.GetBoolean());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Int64, reader.ValueType());
    TestCheckEqual(123, reader.GetInt64());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"abc", reader.GetString());

    TestCheck(!reader.GetNextArrayItem());
  }

  TEST_METHOD(WriteGetObject) {
    IJSValueWriter writer = Microsoft::Internal::TestController::CreateDynamicWriter();

    writer.WriteObjectBegin();

    writer.WritePropertyName(L"Name");
    writer.WriteString(L"Bob");

    writer.WritePropertyName(L"Age");
    writer.WriteInt64(32);

    writer.WritePropertyName(L"Married");
    writer.WriteBoolean(true);

    writer.WriteObjectEnd();

    IJSValueReader reader = Microsoft::Internal::TestController::CreateDynamicReader(writer);
    TestCheckEqual(JSValueType::Object, reader.ValueType());

    hstring propertyName;

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Name", propertyName);
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"Bob", reader.GetString());
    propertyName.clear();

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Age", propertyName);
    TestCheckEqual(JSValueType::Int64, reader.ValueType());
    TestCheckEqual(32, reader.GetInt64());
    propertyName.clear();

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Married", propertyName);
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheckEqual(true, reader.GetBoolean());
    propertyName.clear();

    TestCheck(!reader.GetNextObjectProperty(/* out */ propertyName));
  }

  TEST_METHOD(WriteGetObjectArrayMix) {
    IJSValueWriter writer = Microsoft::Internal::TestController::CreateDynamicWriter();

    writer.WriteObjectBegin();

    writer.WritePropertyName(L"Name");
    writer.WriteString(L"Washington");

    writer.WritePropertyName(L"Counties");
    writer.WriteArrayBegin();
    writer.WriteString(L"Snohomish");
    writer.WriteString(L"King");
    writer.WriteString(L"Pierce");
    writer.WriteArrayEnd();

    writer.WritePropertyName(L"Area");
    writer.WriteInt64(184827);

    writer.WriteObjectEnd();

    IJSValueReader reader = Microsoft::Internal::TestController::CreateDynamicReader(writer);
    TestCheckEqual(JSValueType::Object, reader.ValueType());

    hstring propertyName;

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Name", propertyName);
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"Washington", reader.GetString());
    propertyName.clear();

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Counties", propertyName);
    TestCheckEqual(JSValueType::Array, reader.ValueType());
    propertyName.clear();

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"Snohomish", reader.GetString());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"King", reader.GetString());

    TestCheck(reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::String, reader.ValueType());
    TestCheckEqual(L"Pierce", reader.GetString());

    TestCheck(!reader.GetNextArrayItem());

    TestCheck(reader.GetNextObjectProperty(/* out */ propertyName));
    TestCheckEqual(L"Area", propertyName);
    TestCheckEqual(184827, reader.GetInt64());
    propertyName.clear();

    TestCheck(!reader.GetNextObjectProperty(/**/ propertyName));
  }

 private:
  template <class TWriterValue, class TReaderValue = TWriterValue>
  void TestScalar(
      void (IJSValueWriter::*writerMethod)(TWriterValue) const,
      TReaderValue (IJSValueReader::*readerMethod)() const,
      JSValueType runtimeType,
      TWriterValue value) {
    IJSValueWriter writer = Microsoft::Internal::TestController::CreateDynamicWriter();
    (writer.*writerMethod)(value);

    IJSValueReader reader = Microsoft::Internal::TestController::CreateDynamicReader(writer);
    TestCheckEqual(runtimeType, reader.ValueType());
    TestCheckEqual(value, (reader.*readerMethod)());
  }
};

} // namespace ABITests
