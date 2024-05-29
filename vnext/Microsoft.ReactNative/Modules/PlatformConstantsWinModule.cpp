// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PlatformConstantsWinModule.h"

#include <VersionHelpers.h>
#include <cxxreact/ReactNativeVersion.h>
#include <winrt/Windows.Foundation.Metadata.h>

namespace Microsoft::ReactNative {

ReactNativeSpecs::PlatformConstantsWinSpec_PlatformConstantsWindows PlatformConstants::GetConstants() noexcept {
  ReactNativeSpecs::PlatformConstantsWinSpec_PlatformConstantsWindows constants;

  // We don't currently treat Native code differently in a test environment
  constants.isTesting = false;

  // Provide version information stamped into the compiled version of react-native
  constants.reactNativeVersion.major = facebook::react::ReactNativeVersion.Major;
  constants.reactNativeVersion.minor = facebook::react::ReactNativeVersion.Minor;
  constants.reactNativeVersion.patch = facebook::react::ReactNativeVersion.Patch;
  if (!facebook::react::ReactNativeVersion.Prerelease.empty()) {
    constants.reactNativeVersion.prerelease = facebook::react::ReactNativeVersion.Prerelease;
  }

  // Provide the universal API contract as an OS version
  if (!IsWindows10OrGreater()) {
    constants.osVersion = -1;
  } else {
    for (uint16_t i = 1;; ++i) {
      if (!winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(
              L"Windows.Foundation.UniversalApiContract", i)) {
        constants.osVersion = i - 1;
        break;
      }
    }
  }

  return constants;
}

} // namespace Microsoft::ReactNative
