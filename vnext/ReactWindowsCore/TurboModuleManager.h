#pragma once

#include <ReactCommon/TurboModule.h>
#include <ReactCommon/JSCallInvoker.h>

namespace facebook {
namespace react {

class TurboModuleManager {
 public:
  TurboModuleManager(std::shared_ptr<JSCallInvoker> jsInvoker);

  /**
   * Return the TurboModule instance that has that name `moduleName`. If the `moduleName`
   * TurboModule hasn't been instantiated, instantiate it. If no TurboModule is registered under
   * `moduleName`, return null.
   */
  std::shared_ptr<TurboModule> getModule(const std::string &moduleName);

  /** Get all instantiated TurboModules. */
  // std::vector<std::shared_ptr<TurboModule>>& getModules();

  /** Has the TurboModule with name `moduleName` been instantiated? */
  bool hasModule(const std::string &moduleName);

  /**
   * Return the names of all the NativeModules that are supposed to be eagerly initialized. By
   * calling getModule on each name, this allows the application to eagerly initialize its
   * NativeModules.
   */
  std::vector<std::string> getEagerInitModuleNames();

  void onInstanceDestroy();
  
 private:
  std::unordered_map<std::string, std::shared_ptr<TurboModule>> m_modules;
  std::shared_ptr<JSCallInvoker> m_jsInvoker;

};
} // namespace react
} // namespace facebook}
