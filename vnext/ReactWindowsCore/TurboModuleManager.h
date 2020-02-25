#pragma once

#include <ReactCommon/JSCallInvoker.h>
#include <ReactCommon/TurboModule.h>
#include "TurboModuleRegistry.h"

namespace facebook {
namespace react {

class TurboModuleManager : public TurboModuleRegistry {
 public:
  TurboModuleManager(std::shared_ptr<JSCallInvoker> jsInvoker);

  std::shared_ptr<TurboModule> getModule(const std::string &moduleName) override;
  bool hasModule(const std::string &moduleName) override;
  std::vector<std::string> getEagerInitModuleNames() override;
  void onInstanceDestroy();

 private:
  std::unordered_map<std::string, std::shared_ptr<TurboModule>> m_modules;
  std::shared_ptr<JSCallInvoker> m_jsInvoker;
};
} // namespace react
} // namespace facebook
