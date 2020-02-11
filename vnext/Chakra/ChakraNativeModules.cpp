// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraNativeModules.h"
#include "ChakraValue.h"

#include <glog/logging.h>
#include <string>

namespace facebook {
namespace react {

ChakraNativeModules::ChakraNativeModules(std::shared_ptr<ModuleRegistry> moduleRegistry)
    : m_moduleRegistry(std::move(moduleRegistry)) {}

JsValueRef ChakraNativeModules::getModule(JsValueRef jsName) {
  std::string moduleName = ChakraString::ref(jsName).str();

  const auto it = m_objects.find(moduleName);
  if (it != m_objects.end()) {
    return static_cast<JsValueRef>(it->second);
  }

  auto module = createModule(moduleName);
  if (!module.hasValue()) {
    return ChakraValue::makeUndefined();
  }

  // Protect since we'll be holding on to this value, even though JS may not
  module->makeProtected();

  auto result = m_objects.emplace(std::move(moduleName), std::move(*module)).first;
  return static_cast<JsValueRef>(result->second);
}

void ChakraNativeModules::reset() {
  m_genNativeModuleJS = nullptr;
  m_objects.clear();
}

folly::Optional<ChakraObject> ChakraNativeModules::createModule(const std::string &name) {
  if (!m_genNativeModuleJS) {
    auto global = ChakraObject::getGlobalObject();
    m_genNativeModuleJS = global.getProperty("__fbGenNativeModule").asObject();
    m_genNativeModuleJS->makeProtected();

    // Initialize the module name list, otherwise getModuleConfig won't work
    // TODO (pieterdb): fix this in ModuleRegistry
    m_moduleRegistry->moduleNames();
  }

  auto result = m_moduleRegistry->getConfig(name);
  if (!result.hasValue()) {
    return folly::none;
  }

  ChakraValue moduleInfo = m_genNativeModuleJS->callAsFunction(
      {ChakraValue::fromDynamic(result->config), ChakraValue::makeNumber(static_cast<double>(result->index))});
  CHECK(!moduleInfo.isNull()) << "Module returned from genNativeModule is null";

  return moduleInfo.asObject().getProperty("module").asObject();
}

} // namespace react
} // namespace facebook
