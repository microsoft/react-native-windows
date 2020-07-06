// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>

using namespace winrt::Microsoft::Internal;
using namespace winrt;

namespace ABITests {

TEST_CLASS (TestControllerTests) {
 public:
  TEST_METHOD(HelloWorld) {
    TestCheckEqual(L"Hello World!", TestController::HelloWorld());
  }
};

} // namespace ABITests
