// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <TestInstance.h>
#include <TestRootView.h>

namespace Microsoft::React::Test {

class DesktopTestInstance : public ITestInstance {
  std::shared_ptr<facebook::react::InstanceWrapper> m_instanceWrapper;
  std::unique_ptr<TestRootView> m_rootView;

 public:
  DesktopTestInstance(std::shared_ptr<facebook::react::InstanceWrapper> instanceWrapper) noexcept;

#pragma region ITestInstance members

  void AttachMeasuredRootView(std::string &&appName) noexcept;
  void DetachRootView() noexcept;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;

#pragma endregion ITestInstance members
};

} // namespace Microsoft::React::Test
