// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "PlatformConstantsModule.h"
#include <VersionHelpers.h>

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

      // Since we're out-of-tree, we don't know the exact version of React Native
      // we're paired with. Provide something sane for now, and try to provide a
      // better source of truth later. Tracked by Issue #4073
      {"reactNativeVersion", folly::dynamic::object("major", 0)("minor", 62)("patch", 0)},

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

} // namespace facebook::react
