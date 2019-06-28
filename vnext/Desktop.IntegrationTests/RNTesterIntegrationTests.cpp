// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include "TestRunner.h"

using namespace facebook::react::test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft::VisualStudio::CppUnitTestFramework {

template <>
std::wstring ToString<facebook::react::test::TestStatus>(const facebook::react::test::TestStatus& status)
{
  return ToString(static_cast<unsigned int>(status));
}

} // namespace Microsoft::VisualStudio::CppUnitTestFramework

TEST_CLASS(RNTesterIntegrationTests)
{
  TestRunner m_runner;

  void TestComponent(std::string&& testComponent)
  {
    auto result = m_runner.RunTest("IntegrationTests/IntegrationTestsAppWin", std::move(testComponent));
    Assert::AreNotEqual(TestStatus::Pending, result.Status, result.Message.c_str());
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }

  #pragma region Prototype tests

  TEST_METHOD(Dummy)
  {
    auto result = m_runner.RunTest("IntegrationTests/DummyTest", "DummyTest");
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(Logging)
    //TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(Logging)
  {
    int logCalls{ 0 };
    auto result = m_runner.RunTest("IntegrationTests/LoggingTest", "LoggingTest", [&logCalls](facebook::react::RCTLogLevel logLevel, const char* message)
    {
      if ((strcmp(message, "This is from console.trace") == 0) && (logLevel == facebook::react::RCTLogLevel::Trace)) {
        logCalls++;
      }

      if ((strcmp(message, "This is from console.debug") == 0) && (logLevel == facebook::react::RCTLogLevel::Trace)) {
        logCalls++;
      }

      if ((strcmp(message, "This is from console.info") == 0) && (logLevel == facebook::react::RCTLogLevel::Info)) {
        logCalls++;
      }

      if ((strcmp(message, "This is from console.log") == 0) && (logLevel == facebook::react::RCTLogLevel::Info)) {
        logCalls++;
      }

      if ((strcmp(message, "This is from console.warn") == 0) && (logLevel == facebook::react::RCTLogLevel::Warning)) {
        logCalls++;
      }

      if ((strcmp(message, "This is from console.error") == 0) && (logLevel == facebook::react::RCTLogLevel::Error)) {
        logCalls++;
      }
    });

    // Check that we received all 6 expected logs
    Assert::AreEqual(6, logCalls);
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(LoggingWithNoCallback)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(LoggingWithNoCallback)
  {
    // When running without a logging callbacks, all logs get routed to the default Logging hook.
    // In the TestRunner's Logging hook we interpret all error traces as test failures, which is exactly what we expect in this case
    auto result = m_runner.RunTest("IntegrationTests/LoggingTest", "LoggingTest");
    Assert::AreEqual(TestStatus::Failed, result.Status);
    Assert::AreEqual(L"This is from console.error", result.Message.c_str());
  }

  TEST_METHOD(XHRSample)
  {
    auto result = m_runner.RunTest("IntegrationTests/XHRTest", "XHRTest");
    Assert::AreEqual(TestStatus::Passed, result.Status, result.Message.c_str());
  }

  #pragma endregion

  TEST_METHOD(IntegrationTestHarness)
  {
    TestComponent("IntegrationTestHarnessTest");
  }

  // Timer tests have been disabled in RN. (See RNTesterIntegrationTests.m)
  BEGIN_TEST_METHOD_ATTRIBUTE(Timers)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(Timers)
  {
    TestComponent("TimersTest");
  }

  TEST_METHOD(AsyncStorage)
  {
    TestComponent("AsyncStorageTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(LayoutEvents)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(LayoutEvents)
  {
    TestComponent("LayoutEventsTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(AppEvents)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(AppEvents)
  {
	  TestComponent("AppEventsTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleSnapshot)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleSnapshot)
  {
    TestComponent("SimpleSnapshotTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(ImageCachePolicy)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(ImageCachePolicy)
  {
    TestComponent("ImageCachePolicyTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(ImageSnapshot)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(ImageSnapshot)
  {
    TestComponent("ImageSnapshotTest");
  }

  // Fails with error: Cannot read property 'then'
  // Currently aborts.
  BEGIN_TEST_METHOD_ATTRIBUTE(Promise)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(Promise)
  {
	  TestComponent("PromiseTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(WebView)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(WebView)
  {
    TestComponent("WebViewTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SyncMethod)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SyncMethod)
  {
    TestComponent("SyncMethodTest");
  }

  //ISS:3219193 - Fix intermittent errors, then re-enable.
  BEGIN_TEST_METHOD_ATTRIBUTE(WebSocket)
    TEST_OWNER(L"Unstable")
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(WebSocket)
  {
    TestComponent("WebSocketTest");
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(AccessibilityManager)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(AccessibilityManager)
  {
    TestComponent("AccessibilityManagerTest");
  }
};
