// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <DynamicReader.h>
#include <DynamicWriter.h>
#include "CommonReaderTest.h"

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (DynamicReaderTest) {
  template <typename TCase>
  void RunReaderTest() {
    IJSValueWriter writer = winrt::make<DynamicWriter>();
    TCase::Write(writer);
    auto dynamicValue = writer.as<DynamicWriter>()->TakeValue();
    IJSValueReader reader = winrt::make<DynamicReader>(dynamicValue);
    TCase::Read(reader);
  }

  IMPORT_READER_TEST_CASES
};

} // namespace winrt::Microsoft::ReactNative
