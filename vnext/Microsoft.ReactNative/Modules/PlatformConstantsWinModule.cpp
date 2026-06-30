// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PlatformConstantsWinModule.h"

#include <VersionHelpers.h>
#include <cxxreact/ReactNativeVersion.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.System.Profile.h>

namespace Microsoft::ReactNative {

static std::string GetReleaseVersion() noexcept {
  try {
    const auto versionInfo = winrt::Windows::System::Profile::AnalyticsInfo::VersionInfo();
    const auto version = std::stoull(winrt::to_string(versionInfo.DeviceFamilyVersion()));
    return std::to_string((version & 0xFFFF000000000000ULL) >> 48) + "." +
        std::to_string((version & 0x0000FFFF00000000ULL) >> 32) + "." +
        std::to_string((version & 0x00000000FFFF0000ULL) >> 16) + "." +
        std::to_string(version & 0x000000000000FFFFULL);
  } catch (...) {
    return "";
  }
}

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

  constants.Release = GetReleaseVersion();

  return constants;
}

} // namespace Microsoft::ReactNative
