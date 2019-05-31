// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

#include <ReactWindowsCore/ReactWindowsAPI.h>

namespace facebook { namespace react {

class REACTWINDOWS_EXPORT AppState
{
public:
  AppState();
  virtual ~AppState();

  virtual const char* getState();
};

#pragma warning(push)
#pragma warning(disable: 4275) // base is not DLL exported
#pragma warning(disable: 4251) // member is not DLL exported
class REACTWINDOWS_EXPORT AppStateModule : public facebook::xplat::module::CxxModule
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
#pragma warning(pop)

} } // namespace facebook::react
