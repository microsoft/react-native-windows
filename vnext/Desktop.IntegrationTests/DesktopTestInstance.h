// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <TestInstance.h>

namespace Microsoft::React::Test {

class DesktopTestInstance : public ITestInstance {
  std::shared_ptr<facebook::react::InstanceWrapper> m_instanceWrapper;

 public:
  DesktopTestInstance(std::shared_ptr<facebook::react::InstanceWrapper> instanceWrapper) noexcept;

#pragma region ITestInstance members

  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;

#pragma endregion ITestInstance members
};

} // namespace Microsoft::React::Test
