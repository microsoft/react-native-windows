// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "DesktopTestInstance.h"

using namespace facebook::react;

using std::make_unique;
using std::move;
using std::shared_ptr;

namespace Microsoft::React::Test {

#pragma region DesktopTestInstance members

DesktopTestInstance::DesktopTestInstance(
    shared_ptr<InstanceWrapper> instanceWrapper) noexcept
    : m_instanceWrapper{move(instanceWrapper)} {}

void DesktopTestInstance::AttachMeasuredRootView(
    std::string &&appName) noexcept {
  m_rootView = make_unique<TestRootView>(move(appName));

  m_instanceWrapper->AttachMeasuredRootView(m_rootView.get(), {});
}

void DesktopTestInstance::DetachRootView() noexcept {
  m_instanceWrapper->DetachRootView(m_rootView.get());
}

shared_ptr<facebook::react::Instance> DesktopTestInstance::GetInnerInstance()
    const noexcept {
  return m_instanceWrapper->GetInstance();
}

#pragma endregion DesktopTestInstance members

} // namespace Microsoft::React::Test
