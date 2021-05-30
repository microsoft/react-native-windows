// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "I18nModule.h"
#include <cxxreact/JsArgumentHelpers.h>

using namespace facebook::xplat;

namespace react {
namespace windows {

I18nModule::I18nModule(std::unique_ptr<II18nModule> module) : m_module(std::move(module)) {}

std::string I18nModule::getName() {
  return "I18nManager";
}

std::map<std::string, folly::dynamic> I18nModule::getConstants() {
  return {
      {"localeIdentifier", folly::dynamic{m_module->getLocaleIdentifier()}},
      {"isRTL", folly::dynamic{m_module->getIsRTL()}}};
}

std::vector<facebook::xplat::module::CxxModule::Method> I18nModule::getMethods() {
  return {
      Method("allowRTL", [this](folly::dynamic args) { this->m_module->setAllowRTL(jsArgAsBool(args, 0)); }),
      Method("forceRTL", [this](folly::dynamic args) { this->m_module->setForceRTL(jsArgAsBool(args, 0)); }),
  };
}

std::unique_ptr<facebook::xplat::module::CxxModule> createI18nModule(std::unique_ptr<II18nModule> module) {
  return std::make_unique<I18nModule>(std::move(module));
}

} // namespace windows
} // namespace react
