// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AlertModuleUwp.h"

#include <Utils\ValueUtils.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

//
// Alert
//

const char *AlertModule::name = "Alert";

AlertModule::~AlertModule() = default;

std::string AlertModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> AlertModule::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method>
AlertModule::getMethods() {
  return {};
}

} // namespace uwp
} // namespace react
