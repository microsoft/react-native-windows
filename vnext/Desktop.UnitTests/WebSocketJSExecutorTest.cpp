// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WebSocketJSExecutor.h>

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(WebSocketJSExecutorTest){
    TEST_METHOD(WebSocketJSExecutorTest_Instantiate){
        auto wsje = std::make_shared<WebSocketJSExecutor>(nullptr, nullptr);

Assert::IsFalse(nullptr == wsje);
}
}
;
