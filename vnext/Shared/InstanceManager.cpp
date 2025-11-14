// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <DevSettings.h>
#include <IDevSupportManager.h>

namespace Microsoft::ReactNative {

const std::shared_ptr<facebook::react::IDevSupportManager> &GetSharedDevManager() noexcept {
  static std::shared_ptr<facebook::react::IDevSupportManager> s_devManager(facebook::react::CreateDevSupportManager());

  return s_devManager;
}
} // namespace Microsoft::ReactNative
