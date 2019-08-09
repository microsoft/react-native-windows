// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/II18nModule.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>

namespace react {
namespace windows {

class I18nModule : public facebook::xplat::module::CxxModule {
 public:
  I18nModule(std::unique_ptr<II18nModule> module);

  virtual std::string getName() override;
  virtual std::map<std::string, folly::dynamic> getConstants() override;
  virtual std::vector<Method> getMethods() override;

 private:
  std::unique_ptr<II18nModule> m_module;
};

std::unique_ptr<facebook::xplat::module::CxxModule> createI18nModule(
    std::unique_ptr<II18nModule> module);

} // namespace windows
} // namespace react
