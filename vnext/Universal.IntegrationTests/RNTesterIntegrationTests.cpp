// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>
#include "TestRunner.h"

using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft::VisualStudio::CppUnitTestFramework {

template <>
std::wstring ToString<TestStatus>(const TestStatus &status) {
  return ToString(static_cast<unsigned int>(status));
}

} // namespace Microsoft::VisualStudio::CppUnitTestFramework

TEST_CLASS(RNTesterIntegrationTests) {
  TestRunner m_runner;

  TEST_METHOD(Dummy) {
    auto result = m_runner.RunTest("IntegrationTests/DummyTest", "DummyTest");
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }
};
