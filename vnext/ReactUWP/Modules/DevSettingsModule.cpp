// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DevSettingsModule.h"

namespace react::uwp {

auto DevSettingsModule::getConstants() -> std::map<std::string, folly::dynamic> {
  return {};
}

const char *DevSettingsModule::name = "DevSettings";
std::string DevSettingsModule::getName() {
  return name;
}

DevSettingsModule::DevSettingsModule() {}

void DevSettingsModule::reload() {
  assert(false);
}
void DevSettingsModule::setHotLoadingEnabled(bool /*isHotLoadingEnabled*/) {
  assert(false);
}
void DevSettingsModule::setIsDebuggingRemotely(bool /*isDebuggingRemotelyEnabled*/) {
  assert(false);
}
void DevSettingsModule::setLiveReloadEnabled(bool /*setLiveReloadEnabled*/) {
  assert(false);
}
void DevSettingsModule::setProfilingEnabled(bool /*setProfilingEnabled*/) {
  assert(false);
}
void DevSettingsModule::toggleElementInspector() {
  assert(false);
}

// iOS only.
void DevSettingsModule::setIsShakeToShowDevMenuEnabled(bool /*enabled*/) {
  assert(false);
}

auto DevSettingsModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method> {
  return {
      Method("reload", [this](folly::dynamic args) { reload(); }),
      Method("setHotLoadingEnabled", [this](folly::dynamic args) { setHotLoadingEnabled(args[0].getBool()); }),
      Method("setIsDebuggingRemotely", [this](folly::dynamic args) { setIsDebuggingRemotely(args[0].getBool()); }),
      Method("setLiveReloadEnabled", [this](folly::dynamic args) { setLiveReloadEnabled(args[0].getBool()); }),
      Method("setProfilingEnabled", [this](folly::dynamic args) { setProfilingEnabled(args[0].getBool()); }),
      Method("toggleElementInspector", [this](folly::dynamic args) { toggleElementInspector(); }),
      Method(
          "setIsShakeToShowDevMenuEnabled",
          [this](folly::dynamic args) { setIsShakeToShowDevMenuEnabled(args[0].getBool()); }),
  };
}

} // namespace react::uwp
