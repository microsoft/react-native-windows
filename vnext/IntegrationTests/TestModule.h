// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

namespace facebook {
namespace react {
namespace test {

class TestModule : public facebook::xplat::module::CxxModule
{
  std::function<void(bool)> m_testPassedHandler;

public:
  static std::string name;

  TestModule(std::function<void(bool)>&& testPassedHandler);

  #pragma region TestModule.js methods

  void sendAppEvent(const std::string& name);

  void markTestPassed(bool success);

  void markTestCompleted();

  void shouldResolve();

  #pragma endregion TestModule.js methods

  #pragma region CxxModule members

  std::string getName() override;

  std::map<std::string, folly::dynamic> getConstants() override;

  std::vector<Method> getMethods() override;

  #pragma endregion CxxModule members
};

class TestDeviceInfoModule : public facebook::xplat::module::CxxModule
{
public:
  static std::string name;

  TestDeviceInfoModule();

  std::string getName() override;

  auto getConstants()->std::map<std::string, folly::dynamic> override;

  auto getMethods()->std::vector<Method> override;
};

} } } // namespace facebook::react::test
