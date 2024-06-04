// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PlatformConstantsWinModule.h"

#include <VersionHelpers.h>
#include <cxxreact/ReactNativeVersion.h>
#include <winrt/Windows.Foundation.Metadata.h>

namespace Microsoft::ReactNative {

ReactNativeSpecs::PlatformConstantsWindowsSpec_PlatformConstantsWindows PlatformConstants::GetConstants() noexcept {
  ReactNativeSpecs::PlatformConstantsWindowsSpec_PlatformConstantsWindows constants;

  // We don't currently treat Native code differently in a test environment
  constants.isTesting = false;

  // Provide version information stamped into the compiled version of react-native
  constants.reactNativeVersion.major = facebook::react::ReactNativeVersion.Major;
  constants.reactNativeVersion.minor = facebook::react::ReactNativeVersion.Minor;
  constants.reactNativeVersion.patch = facebook::react::ReactNativeVersion.Patch;
  if (!facebook::react::ReactNativeVersion.Prerelease.empty()) {
    constants.reactNativeVersion.prerelease = facebook::react::ReactNativeVersion.Prerelease;
  }

  // Provide version information for react-native-windows -- which is independent of
  // the version of react-native we are built from
  constants.reactNativeWindowsVersion.major = RNW_MAJOR;
  constants.reactNativeWindowsVersion.minor = RNW_MINOR;
  constants.reactNativeWindowsVersion.patch = RNW_PATCH;

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
