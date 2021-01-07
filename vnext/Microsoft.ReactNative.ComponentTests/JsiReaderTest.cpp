// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <JSI/ChakraRuntime.h>
#include <JsiReader.h>
#include <JsiWriter.h>
#include "CommonReaderTest.h"

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (JsiReaderTest) {
  ::Microsoft::JSI::ChakraRuntime m_runtime;

  JsiReaderTest() : m_runtime({}) {}

  template <typename TCase>
  void RunReaderTest() {
    IJSValueWriter writer = winrt::make<JsiWriter>(m_runtime);
    TCase::Write(writer);
    IJSValueReader reader = winrt::make<JsiReader>(m_runtime, writer.as<JsiWriter>()->MoveResult());
    TCase::Read(reader);
  }

  IMPORT_READER_TEST_CASES
};

} // namespace winrt::Microsoft::ReactNative
