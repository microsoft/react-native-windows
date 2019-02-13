// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>
#include <Windows.h>
#include <memory>
#include <string>
#include "TestInstance.h"

namespace facebook {
namespace react {
namespace test {

enum class TestStatus : unsigned int
{
  Pending = 0,
  Passed,
  Failed
};

struct TestResult
{
  TestStatus Status;
  std::wstring Message;
};

class TestRunner
{
  void AwaitEvent(HANDLE& event, TestResult& status);

  std::shared_ptr<ITestInstance> GetInstance(
    std::string&& jsBundleFile,
    std::vector<std::tuple<std::string, facebook::xplat::module::CxxModule::Provider>>&& cxxModules,
    std::shared_ptr<DevSettings> devSettings) noexcept;

public:
  TestRunner();

  TestResult RunTest(std::string&& bundlePath, std::string&& appName, NativeLoggingHook&& loggingCallback = {});
};

} } } // namespace facebook::react::test
