// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

// AppModel helpers

#include <appmodel.h>

namespace Microsoft::ReactNative {

inline bool HasPackageIdentity() noexcept {
  uint32_t length{0};
  const bool isPackagedApp = GetCurrentPackageFullName(&length, nullptr) != APPMODEL_ERROR_NO_PACKAGE;
  return isPackagedApp;
}

} // namespace Microsoft::ReactNative
