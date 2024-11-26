// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TurboModuleManager.h"
#include "ReactCommon/SampleTurboCxxModule.h"

#ifdef USE_FABRIC
#include <react/nativemodule/defaults/DefaultTurboModules.h>
#include <react/nativemodule/microtasks/NativeMicrotasks.h>
#endif

namespace facebook::react {

#ifdef USE_FABRIC

NativeMicrotasks::NativeMicrotasks(std::shared_ptr<CallInvoker> jsInvoker)
    : NativeMicrotasksCxxSpec(std::move(jsInvoker)) {}

void NativeMicrotasks::queueMicrotask(jsi::Runtime &runtime, jsi::Function callback) {
  assert(false);
  // Currently using a version of hermes/jsi without queueMicrotask
  // When we update JSI we can remove all the Microtask code from here, and just rely on the implementation of
  // the NativeMicrotasks TurboModule from the core NativeMicrotasks.cpp file
  // runtime.queueMicrotask(callback);
}

#endif

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

#ifdef USE_FABRIC
  if (auto module = facebook::react::DefaultTurboModules::getTurboModule(moduleName, m_callInvoker)) {
    m_modules.emplace(moduleName, module);
    return module;
  }
#endif

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
  // TODO - Provide a story to notify TurboModules of instance destroy
  /*
  for (const auto &it : m_modules) {
    it.second->onInstanceDestroy();
  }
  */

  m_modules.clear();
}

} // namespace facebook::react
