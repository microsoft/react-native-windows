// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "PlatformConstantsModule.h"
#include <VersionHelpers.h>

using Method = facebook::xplat::module::CxxModule::Method;

namespace facebook::react {

const char *PlatformConstantsModule::name = "PlatformConstants";

std::string PlatformConstantsModule::getName() {
  return PlatformConstantsModule::name;
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
    {"reactNativeVersion", folly::dynamic::object("major", 0)("minor", 61)("patch", 0)}

    // Intentionally do not provide an OS version constant. Windows discourages
    // testing for real version, and universal API contract isn't quite accurate
    // here either since we can run on pre-10 and need to use hacks to query it.
  };
}

} // namespace facebook::react
