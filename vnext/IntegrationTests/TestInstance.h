// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/ReactMarker.h>
#include <folly/dynamic.h>

#include <queue>

namespace Microsoft::React::Test {

struct ITestInstance {
  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept = 0;
};

class TestUIManager : public facebook::xplat::module::CxxModule {
 public:
  TestUIManager() noexcept;
  ~TestUIManager() noexcept override;

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;
};

} // namespace Microsoft::React::Test
