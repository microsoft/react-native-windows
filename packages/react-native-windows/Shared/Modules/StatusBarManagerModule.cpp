// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "StatusBarManagerModule.h"

using Method = facebook::xplat::module::CxxModule::Method;

namespace facebook::react {

std::string StatusBarManagerModule::getName() {
  return StatusBarManagerModule::Name;
}

std::vector<Method> StatusBarManagerModule::getMethods() {
  return {
      Method("setStyle", [](folly::dynamic /*args*/) noexcept {}),
      Method("setHidden", [](folly::dynamic /*args*/) noexcept {}),
  };
}

std::map<std::string, folly::dynamic> StatusBarManagerModule::getConstants() {
  return {{"HEIGHT", 0}};
}

} // namespace facebook::react
