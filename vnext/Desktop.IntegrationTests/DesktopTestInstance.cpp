// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DesktopTestInstance.h"

using namespace facebook::react;

using std::make_unique;
using std::move;
using std::shared_ptr;

namespace Microsoft::React::Test {

#pragma region DesktopTestInstance members

DesktopTestInstance::DesktopTestInstance(shared_ptr<InstanceWrapper> instanceWrapper) noexcept
    : m_instanceWrapper{move(instanceWrapper)} {}

shared_ptr<facebook::react::Instance> DesktopTestInstance::GetInnerInstance() const noexcept {
  return m_instanceWrapper->GetInstance();
}

#pragma endregion DesktopTestInstance members

} // namespace Microsoft::React::Test
