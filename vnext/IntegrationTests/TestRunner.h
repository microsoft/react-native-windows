// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>
#include <Windows.h>
#include <memory>
#include <string>
#include "TestInstance.h"

namespace Microsoft::React::Test {

enum class TestStatus : unsigned int { Pending = 0, Passed, Failed };

struct TestResult {
  TestStatus Status{TestStatus::Pending};
  std::wstring Message;
};

class TestRunner {
  void AwaitEvent(HANDLE &event, TestResult &status);

  std::shared_ptr<ITestInstance> GetInstance(
      std::string &&jsBundleFile,
      std::vector<std::tuple<std::string, facebook::xplat::module::CxxModule::Provider>> &&cxxModules,
      std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept;

 public:
  TestRunner();

  TestResult
  RunTest(std::string &&bundlePath, std::string &&appName, facebook::react::NativeLoggingHook &&loggingCallback = {});
};

} // namespace Microsoft::React::Test
