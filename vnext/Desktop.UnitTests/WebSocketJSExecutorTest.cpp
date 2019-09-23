// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <Executors/WebSocketJSExecutor.h>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft::React::Test {

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(WebSocketJSExecutorTest) {
  TEST_METHOD(WebSocketJSExecutorTest_Instantiate){
    auto wsje = std::make_shared<WebSocketJSExecutor>(nullptr, nullptr);
    Assert::IsFalse(nullptr == wsje);
  }
};

// clange-format on

} // namespace Microsoft::React::Test
