// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/Optional.h>

#include <memory>
#include <string>

#include "ChakraValue.h"

#include <cxxreact\ModuleRegistry.h>

namespace facebook {
namespace react {

/**
 * Holds and creates JS representations of the modules in ModuleRegistry
 */
class ChakraNativeModules {
 public:
  explicit ChakraNativeModules(std::shared_ptr<ModuleRegistry> moduleRegistry);
  JsValueRef getModule(JsValueRef name);
  void reset();

 private:
  folly::Optional<ChakraObject> m_genNativeModuleJS;
  std::shared_ptr<ModuleRegistry> m_moduleRegistry;
  std::unordered_map<std::string, ChakraObject> m_objects;

  folly::Optional<ChakraObject> createModule(const std::string &name);
};

} // namespace react
} // namespace facebook
