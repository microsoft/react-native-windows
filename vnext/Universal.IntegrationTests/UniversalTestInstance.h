// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include <ReactUWP/IReactInstance.h>
#include <IXamlRootView.h>
#include <TestInstance.h>

namespace Microsoft::React::Test
{

class UniversalTestInstance : public ITestInstance
{
  std::shared_ptr<::react::uwp::IReactInstance> m_instance;
  std::shared_ptr<::react::uwp::IXamlRootView> m_rootView;

public:
  UniversalTestInstance(std::shared_ptr<::react::uwp::IReactInstance> instance) noexcept;

  #pragma region ITestInstance members

  void AttachMeasuredRootView(std::string&& appName) noexcept;
  void DetachRootView() noexcept;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;

  #pragma endregion ITestInstance members
private:
  ::react::uwp::ReactInstanceCreator m_instanceCreator;
};

} // namespace Microsoft::React::Test
