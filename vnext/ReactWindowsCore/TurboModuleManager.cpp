#pragma once

#include "TurboModuleManager.h"
#include "turbomodule/samples/SampleTurboCxxModule.h"

namespace facebook {
namespace react {

TurboModuleManager::TurboModuleManager(
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<JSCallInvoker> jsInvoker)
    : m_turboModuleRegistry(turboModuleRegistry), m_jsInvoker(jsInvoker) {}

/**
 * Return the TurboModule instance that has that name `moduleName`. If the `moduleName`
 * TurboModule hasn't been instantiated, instantiate it. If no TurboModule is registered under
 * `moduleName`, return null.
 */
std::shared_ptr<TurboModule> TurboModuleManager::getModule(const std::string &moduleName) {
  if (!hasModule(moduleName)) {
    std::shared_ptr<TurboModule> module;

    if (m_turboModuleRegistry) {
      module = m_turboModuleRegistry->getModule(moduleName, m_jsInvoker);
    }

    if (module) {
      m_modules.emplace(moduleName, module);
    } else if (moduleName.compare("SampleTurboModule") == 0) {
      m_modules.emplace(moduleName, std::make_shared<SampleTurboCxxModule>(m_jsInvoker));
    }
  }

  const auto &it = m_modules.find(moduleName);
  return it == m_modules.end() ? nullptr : it->second;
}

/** Get all instantiated TurboModules. */
/*
std::vector<std::shared_ptr<TurboModule>> &TurboModuleManager::getModules() {
  return {};
}
*/

/** Has the TurboModule with name `moduleName` been instantiated? */
bool TurboModuleManager::hasModule(const std::string &moduleName) {
  return m_modules.find(moduleName) != m_modules.end();
}

/**
 * Return the names of all the NativeModules that are supposed to be eagerly initialized. By
 * calling getModule on each name, this allows the application to eagerly initialize its
 * NativeModules.
 */
std::vector<std::string> TurboModuleManager::getEagerInitModuleNames() {
  return m_turboModuleRegistry ? m_turboModuleRegistry->getEagerInitModuleNames() : std::vector<std::string>{};
}

void TurboModuleManager::onInstanceDestroy() {
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
