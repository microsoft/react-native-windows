// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "I18nModule.h"

namespace react {
namespace windows {

I18nModule::I18nModule(std::unique_ptr<II18nModule> module)
    : m_module(std::move(module)) {}

std::string I18nModule::getName() {
  return "I18nManager";
}

std::map<std::string, folly::dynamic> I18nModule::getConstants() {
  return {{"localeIdentifier", folly::dynamic{m_module->getLocaleIdentifier()}},
          {"isRTL", folly::dynamic{m_module->getIsRTL()}}};
}

std::vector<facebook::xplat::module::CxxModule::Method>
I18nModule::getMethods() {
  return {};
}

std::unique_ptr<facebook::xplat::module::CxxModule> createI18nModule(
    std::unique_ptr<II18nModule> module) {
  return std::make_unique<I18nModule>(std::move(module));
}

} // namespace windows
} // namespace react