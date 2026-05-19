// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PlatformConstantsWinModule.h"

#include <VersionHelpers.h>
#include <cstdint>
#include <cxxreact/ReactNativeVersion.h>
#include <string>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.System.Profile.h>

namespace Microsoft::ReactNative {

namespace {

std::string GetReleaseVersion() noexcept {
  try {
    std::uint64_t deviceFamilyVersion =
        std::stoull(winrt::to_string(
            winrt::Windows::System::Profile::AnalyticsInfo::VersionInfo()
                .DeviceFamilyVersion()));

    uint16_t major =
        static_cast<uint16_t>((deviceFamilyVersion & 0xFFFF000000000000ULL) >> 48);
    uint16_t minor =
        static_cast<uint16_t>((deviceFamilyVersion & 0x0000FFFF00000000ULL) >> 32);
    uint16_t build =
        static_cast<uint16_t>((deviceFamilyVersion & 0x00000000FFFF0000ULL) >> 16);
    uint16_t revision =
        static_cast<uint16_t>(deviceFamilyVersion & 0x000000000000FFFFULL);

    return std::to_string(major) + "." + std::to_string(minor) + "." +
        std::to_string(build) + "." +
        std::to_string(revision);
  } catch (...) {
    return "";
  }
}

} // namespace

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

  constants.Release = GetReleaseVersion();

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
