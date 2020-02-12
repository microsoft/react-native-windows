// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SourceCodeModule.h"

using namespace folly;
using namespace facebook::xplat;

namespace facebook {
namespace react {

const char *SourceCodeModule::Name = "SourceCode";

SourceCodeModule::SourceCodeModule(const std::string &bundleUrl) : m_bundleUrl(bundleUrl) {}

std::string SourceCodeModule::getName() {
  return Name;
}

auto SourceCodeModule::getConstants() -> std::map<std::string, dynamic> {
  return {{"scriptURL", m_bundleUrl.empty() ? "" : m_bundleUrl}};
}

auto SourceCodeModule::getMethods() -> std::vector<Method> {
  return {};
}

} // namespace react
} // namespace facebook

// By convention, the function name should be the same as the class
// name.
extern "C" facebook::xplat::module::CxxModule *SourceCodeModule() {
  return new facebook::react::SourceCodeModule(std::string());
}
