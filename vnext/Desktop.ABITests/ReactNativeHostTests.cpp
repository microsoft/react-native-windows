// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include <winrt/Microsoft.ReactNative.h>
#include "ActivationFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ABITests {

TEST_CLASS (ReactNativeHostTests) {
 public:
  ReactNativeHostTests() noexcept {
    winrt_activation_handler = WINRT_RoGetActivationFactory;
  }

  TEST_METHOD(Activation_Succeeds) {
    try {
      winrt::Microsoft::ReactNative::ReactNativeHost host{};
      Assert::IsTrue(true);
    } catch (...) {
      Assert::Fail();
    }
  }
};

} // namespace ABITests
