// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <JSI/ChakraRuntimeArgs.h>
#include <JSI/ChakraRuntimeFactory.h>
#include <JsiReader.h>
#include <JsiWriter.h>
#include "CommonReaderTest.h"

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (JsiArgumentReaderTest) {
  std::unique_ptr<::facebook::jsi::Runtime> m_runtime;

  JsiArgumentReaderTest() : m_runtime(::Microsoft::JSI::makeChakraRuntime(::Microsoft::JSI::ChakraRuntimeArgs{})) {}

  template <typename TCase>
  void RunReaderTest() {
    IJSValueWriter writer = winrt::make<JsiWriter>(*m_runtime);
    TCase::Write(writer);
    const facebook::jsi::Value *args = nullptr;
    size_t count = 0;
    writer.as<JsiWriter>()->AccessResultAsArgs(args, count);
    IJSValueReader reader = winrt::make<JsiReader>(*m_runtime, args, count);
    TCase::Read(reader);
  }

  IMPORT_ARGUMENT_READER_TEST_CASES
};

} // namespace winrt::Microsoft::ReactNative
