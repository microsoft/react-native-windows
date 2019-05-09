// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>
#include "TestRunner.h"

using namespace facebook::react::test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
namespace VisualStudio {
namespace CppUnitTestFramework {

template <>
std::wstring ToString<facebook::react::test::TestStatus>(const facebook::react::test::TestStatus& status)
{
  return ToString(static_cast<unsigned int>(status));
}

} } } // namespace Microsoft::VisualStudio::CppUnitTestFramework

TEST_CLASS(RNTesterIntegrationTests)
{
  TestRunner m_runner;

  TEST_METHOD(Dummy)
  {
    auto result = m_runner.RunTest("IntegrationTests/DummyTest", "DummyTest");
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }
};
