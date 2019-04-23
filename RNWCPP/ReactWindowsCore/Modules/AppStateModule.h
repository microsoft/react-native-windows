// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

namespace facebook { namespace react {

class AppState
{
public:
  AppState();
  virtual ~AppState();

  virtual const char* getState();
};

class AppStateModule : public facebook::xplat::module::CxxModule
{
public:
  AppStateModule(std::shared_ptr<AppState>&& appState);

  static const char* name;

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

private:
  std::shared_ptr<AppState> m_appState;
};

} } // namespace facebook::react
