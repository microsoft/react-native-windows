// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#undef max
#undef min

namespace winrt::Microsoft::ReactNative {

TEST_CLASS (DynamicReaderTest) {
  TEST_METHOD(EmptyTest) {
    TestCheckEqual(1, 1);
  }
};

} // namespace winrt::Microsoft::ReactNative
