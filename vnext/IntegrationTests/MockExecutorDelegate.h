// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSExecutor.h>

namespace facebook {
namespace react {
namespace test {

class MockDelegate : public facebook::react::ExecutorDelegate
{
public:
  std::shared_ptr<facebook::react::ModuleRegistry> getModuleRegistry() override
  {
    return nullptr;
  }

  void callNativeModules(facebook::react::JSExecutor& executor, folly::dynamic&& calls, bool isEndOfBatch) override
  {
  }

  facebook::react::MethodCallResult callSerializableNativeHook(facebook::react::JSExecutor& executor, unsigned int moduleId, unsigned int methodId, folly::dynamic&& args) override
  {
    return facebook::react::MethodCallResult();
  }

  bool isBatchActive() override { return false; }
};

} } }// namespace facebook::react::test
