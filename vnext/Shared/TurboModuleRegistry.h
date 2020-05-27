#pragma once

#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/TurboModule.h>

namespace facebook {
namespace react {

class TurboModuleRegistry {
 public:
  /**
   * Return the TurboModule instance that has that name `moduleName`. If the `moduleName`
   * TurboModule hasn't been instantiated, instantiate it. If no TurboModule is registered under
   * `moduleName`, return null.
   */
  virtual std::shared_ptr<TurboModule> getModule(
      const std::string &moduleName,
      const std::shared_ptr<CallInvoker> &callInvoker) noexcept = 0;

  /**
   * Return the names of all the NativeModules that are supposed to be eagerly initialized. By
   * calling getModule on each name, this allows the application to eagerly initialize its
   * NativeModules.
   */
  virtual std::vector<std::string> getEagerInitModuleNames() noexcept = 0;
};

} // namespace react
} // namespace facebook
