// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "StatusBarModule.h"

namespace react {
namespace uwp {

//
// StatusBarModule
//
const char *StatusBarModule::name = "StatusBarManager";

StatusBarModule::~StatusBarModule() = default;

std::string StatusBarModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> StatusBarModule::getConstants() {
  return {{"HEIGHT", 0}};
}

std::vector<facebook::xplat::module::CxxModule::Method> StatusBarModule::getMethods() {
  return {};
}

} // namespace uwp
} // namespace react
