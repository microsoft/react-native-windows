// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <ChakraRuntime.h>
#include <JsiWriter.h>
#include <JsiReader.h>

#undef max
#undef min

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (JsiReaderTest) {
  TEST_METHOD(DummyTest) {
    // just to make sure the code can execute
    // this will be deleted and replaces by real test cases in the next pull request

    ::Microsoft::JSI::ChakraRuntime runtime(::Microsoft::JSI::ChakraRuntimeArgs{});

    IJSValueWriter writer = winrt::make<JsiWriter>(runtime);
    writer.WriteArrayBegin();
    writer.WriteBoolean(true);
    writer.WriteInt64(0);
    writer.WriteArrayEnd();

    IJSValueReader reader = winrt::make<JsiReader>(runtime, writer.as<JsiWriter>()->MoveResult());

    TestCheckEqual(JSValueType::Array, reader.ValueType());
    TestCheckEqual(true, reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Boolean, reader.ValueType());
    TestCheckEqual(true, reader.GetBoolean());
    TestCheckEqual(true, reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Int64, reader.ValueType());
    TestCheckEqual(0, reader.GetInt64());
    TestCheckEqual(false, reader.GetNextArrayItem());
    TestCheckEqual(JSValueType::Null, reader.ValueType());
  }
};

} // namespace winrt::Microsoft::ReactNative
