// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "PlatformConstantsModule.h"
#include <VersionHelpers.h>
#include <cxxreact/ReactNativeVersion.h>

using Method = facebook::xplat::module::CxxModule::Method;

#ifndef RNW_PKG_VERSION_MAJOR
#define RNW_PKG_VERSION_MAJOR 1000
#endif

#ifndef RNW_PKG_VERSION_MINOR
#define RNW_PKG_VERSION_MINOR 0
#endif

#ifndef RNW_PKG_VERSION_PATCH
#define RNW_PKG_VERSION_PATCH 0
#endif

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
      {"reactNativeWindowsVersion",
       folly::dynamic::object("major", RNW_PKG_VERSION_MAJOR)("minor", RNW_PKG_VERSION_MINOR)(
           "patch", RNW_PKG_VERSION_PATCH)}

      // We don't provide the typical OS version here. Windows make it hard to
      // get an exact version by-design. In the future we should consider
      // exposing something here like a facility to check Universal API
      // Contract.
  };
}

folly::dynamic PlatformConstantsModule::StringOrNull(std::string_view str) noexcept {
  return str.empty() ? folly::dynamic(nullptr) : folly::dynamic(str);
}

} // namespace facebook::react
