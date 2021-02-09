// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestRunner.h"
#include "Unicode.h"

#include <DevSettings.h>
#include <InstanceManager.h>
#include <RuntimeOptions.h>
#include <cxxreact/Instance.h>
#include "TestInstance.h"
#include "TestModule.h"

#include <boost/exception/diagnostic_information.hpp>

using namespace facebook::react;
using namespace facebook::xplat::module;
using namespace folly;

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React::Test {

void TestRunner::AwaitEvent(HANDLE &event, TestResult &result) {
  DWORD done = ::WaitForSingleObject(event, INFINITE);

  if (WAIT_OBJECT_0 != done)
    result.Status = TestStatus::Failed;
}

TestRunner::TestRunner() {}

struct TestRedBoxHandler : Mso::React::IRedBoxHandler {
  TestRedBoxHandler(std::function<void(Mso::React::ErrorInfo &&, Mso::React::ErrorType)> &&OnErrorFn)
      : m_onErrorFn(std::move(OnErrorFn)) {}

  void showNewError(Mso::React::ErrorInfo &&info, Mso::React::ErrorType type) override {
    m_onErrorFn(std::move(info), type);
  }
  bool isDevSupportEnabled() const override {
    return true;
  }
  void updateError(Mso::React::ErrorInfo &&) override {}
  void dismissRedbox() override {}

 private:
  std::function<void(Mso::React::ErrorInfo &&, Mso::React::ErrorType)> m_onErrorFn;
};

TestResult TestRunner::RunTest(string &&bundlePath, string &&appName, NativeLoggingHook &&loggingCallback) {
  // Set up
  HANDLE functionCalled = CreateEvent(
      /* lpEventAttributes */ NULL,
      /* bManualReset      */ FALSE,
      /* bInitialState     */ FALSE,
      /* lpName            */ nullptr);
  assert(functionCalled != NULL);
  TestResult result{TestStatus::Pending, {} /*message*/};

  try {
    // TODO: Defer MessageQueueThread creation to variant implementations
    // (Win32, WinRT).
    vector<tuple<string, CxxModule::Provider>> modules{
        {TestModule::name, [this, &result, &functionCalled]() -> unique_ptr<CxxModule> {
           return std::make_unique<TestModule>([this, &result, &functionCalled](bool success) {
             if (TestStatus::Pending != result.Status)
               return;

             result.Status = success ? TestStatus::Passed : TestStatus::Failed;
             result.Message = L"markTestSucceeded(false)";
             ::SetEvent(functionCalled);
           });
         }}};

    // Note, further configuration should be done in each Windows variant's
    // TestRunner implementation.
    shared_ptr<DevSettings> devSettings = make_shared<DevSettings>();
    devSettings->useWebDebugger = GetRuntimeOptionBool("RNTester.UseWebDebugger");
    devSettings->liveReloadCallback = []() {}; // Enables ChakraExecutor
    devSettings->errorCallback = [&result](string message) {
      result.Message = Microsoft::Common::Unicode::Utf8ToUtf16(message);
      result.Status = TestStatus::Failed;
    };
    devSettings->redboxHandler =
        make_shared<TestRedBoxHandler>([&result](Mso::React::ErrorInfo &&info, Mso::React::ErrorType) {
          // The logging test fires this error by design, which shouldn't fail the tests
          if (info.Message.find("console.error: This is from console.error") != string::npos) {
            return;
          }

          result.Message = Microsoft::Common::Unicode::Utf8ToUtf16(info.Message);
          result.Status = TestStatus::Failed;
        });
    devSettings->loggingCallback = std::move(loggingCallback);

    // React instance scope
    {
      shared_ptr<ITestInstance> instance =
          GetInstance(std::move(bundlePath), std::move(modules), std::move(devSettings));

      InitializeLogging([&result, &functionCalled](RCTLogLevel logLevel, const char *message) {
        if (TestStatus::Pending != result.Status)
          return;

        switch (logLevel) {
          case RCTLogLevel::Error:
          case RCTLogLevel::Fatal:
            result.Message = Microsoft::Common::Unicode::Utf8ToUtf16(message);
            result.Status = TestStatus::Failed;
            ::SetEvent(functionCalled);
            break;

          case RCTLogLevel::Info:
          case RCTLogLevel::Trace:
          case RCTLogLevel::Warning:
          default:
            break;
        }
      });

      const int rootTag = 101;
      dynamic params =
          dynamic::array(std::move(appName), dynamic::object("initialProps", dynamic::object())("rootTag", rootTag));
      instance->GetInnerInstance()->callJSFunction("AppRegistry", "runApplication", std::move(params));

      // Failure on instance creation.
      if (TestStatus::Failed == result.Status)
        return result;

      AwaitEvent(functionCalled, result);

      instance->GetInnerInstance()->callJSFunction(
          "AppRegistry", "unmountApplicationComponentAtRootTag", std::move(dynamic::array(rootTag)));
    }

    // Tear down
    CloseHandle(functionCalled);
    functionCalled = NULL;
  } catch (...) {
    result.Message =
        Microsoft::Common::Unicode::Utf8ToUtf16(boost::current_exception_diagnostic_information(/*verbose*/ true));
    result.Status = TestStatus::Failed;
  }
  return result;
}

} // namespace Microsoft::React::Test
