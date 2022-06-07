// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "PlatformConstantsModule.h"
#include <VersionHelpers.h>
#include <cxxreact/ReactNativeVersion.h>
#include <winrt/Windows.Foundation.Metadata.h>

using namespace winrt::Windows::Foundation::Metadata;

using Method = facebook::xplat::module::CxxModule::Method;

namespace facebook::react {

const char *PlatformConstantsModule::Name = "PlatformConstants";

std::string PlatformConstantsModule::getName() {
  return PlatformConstantsModule::Name;
}

std::vector<Method> PlatformConstantsModule::getMethods() {
  return {};
}

std::map<std::string, folly::dynamic> PlatformConstantsModule::getConstants() {
  return {
      // We don't currently treat Native code differently in a test environment
      {"isTesting", false},

      // Provide version information stamped into the compiled version of react-native
      {"reactNativeVersion",
       folly::dynamic::object("major", ReactNativeVersion.Major)("minor", ReactNativeVersion.Minor)(
           "patch", ReactNativeVersion.Patch)("prerelease", StringOrNull(ReactNativeVersion.Prerelease))},

      // Provide version information for react-native-windows -- which is independant of
      // the version of react-native we are built from
      {"reactNativeWindowsVersion", folly::dynamic::object("major", RNW_MAJOR)("minor", RNW_MINOR)("patch", RNW_PATCH)},

      // Provide the universal API contract as an OS version
      {"osVersion", OsVersion()},
  };
}

/*static*/ folly::dynamic PlatformConstantsModule::StringOrNull(std::string_view str) noexcept {
  return str.empty() ? folly::dynamic(nullptr) : folly::dynamic(str);
}

/*static*/ int32_t PlatformConstantsModule::OsVersion() noexcept {
  if (!IsWindows10OrGreater()) {
    return -1;
  }

  for (uint16_t i = 1;; ++i) {
    if (!ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", i)) {
      return i - 1;
    }
  }
}

} // namespace facebook::react
