// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>
#include <IRedBoxHandler.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

namespace facebook {
namespace react {

class ExceptionsManagerModule : public facebook::xplat::module::CxxModule {
 public:
  ExceptionsManagerModule(std::shared_ptr<Mso::React::IRedBoxHandler> redboxHandler);

  static constexpr const char *name = "ExceptionsManager";

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

 private:
  std::shared_ptr<Mso::React::IRedBoxHandler> m_redboxHandler;
};

} // namespace react
} // namespace facebook
