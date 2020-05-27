#pragma once

#include "TurboModuleManager.h"
#include "turbomodule/samples/SampleTurboCxxModule.h"

namespace facebook {
namespace react {

TurboModuleManager::TurboModuleManager(
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<CallInvoker> callInvoker)
    : m_turboModuleRegistry(std::move(turboModuleRegistry)), m_callInvoker(std::move(callInvoker)) {}

/**
 * Return the TurboModule instance that has that name `moduleName`. If the `moduleName`
 * TurboModule hasn't been instantiated, instantiate it. If no TurboModule is registered under
 * `moduleName`, return null.
 */
std::shared_ptr<TurboModule> TurboModuleManager::getModule(const std::string &moduleName) noexcept {
  const auto it = m_modules.find(moduleName);

  if (it != m_modules.end())
    return it->second;

  if (m_turboModuleRegistry) {
    if (auto module = m_turboModuleRegistry->getModule(moduleName, m_callInvoker)) {
      m_modules.emplace(moduleName, module);
      return module;
    }
  }

  if (moduleName.compare("SampleTurboModule") == 0) {
    return m_modules.emplace(moduleName, std::make_shared<SampleTurboCxxModule>(m_callInvoker)).first->second;
  }
  return nullptr;
}

/** Get all instantiated TurboModules. */
/*
std::vector<std::shared_ptr<TurboModule>> &TurboModuleManager::getModules() {
  return {};
}
*/

/** Has the TurboModule with name `moduleName` been instantiated? */
bool TurboModuleManager::hasModule(const std::string &moduleName) noexcept {
  return m_modules.find(moduleName) != m_modules.end();
}

/**
 * Return the names of all the NativeModules that are supposed to be eagerly initialized. By
 * calling getModule on each name, this allows the application to eagerly initialize its
 * NativeModules.
 */
std::vector<std::string> TurboModuleManager::getEagerInitModuleNames() noexcept {
  return m_turboModuleRegistry ? m_turboModuleRegistry->getEagerInitModuleNames() : std::vector<std::string>{};
}

void TurboModuleManager::onInstanceDestroy() noexcept {
  // TODO - Provide a story to notify TurboModules of instance destory
  /*
  for (const auto &it : m_modules) {
    it.second->onInstanceDestroy();
  }
  */

  m_modules.clear();
}

} // namespace react
} // namespace facebook
