// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulesProvider.h"
#include "ABICxxModule.h"
#include "ABIModule.h"
#include "IReactModuleBuilder.h"

#include <ReactUWP/ReactUwp.h>
#include <folly/json.h>

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::Bridge {
/*-------------------------------------------------------------------------------
  NativeModulesProvider::GetModules
-------------------------------------------------------------------------------*/
std::vector<facebook::react::NativeModuleDescription>
NativeModulesProvider::GetModules(
    std::shared_ptr<facebook::react::MessageQueueThread> const
        & /*defaultQueueThread*/) {
  // std::shared_ptr<facebook::react::MessageQueueThread>
  // queueThread(defaultQueueThread);
  std::vector<facebook::react::NativeModuleDescription> modules;
  modules.reserve(m_modules.size());

  if (m_modulesWorkerQueue == nullptr) {
    // TODO: The queue provided is the UIMessageQueueThread which isn't needed
    // for native modules. As a workaround for now let's just use a new worker
    // message queue.
    m_modulesWorkerQueue = react::uwp::CreateWorkerMessageQueue();
  }

  for (auto &module : m_modules) {
    modules.emplace_back(
        winrt::to_string(module.Name()),
        [module]() { return std::make_unique<ABIModule>(module); },
        m_modulesWorkerQueue);
  }

  for (auto &entry : m_moduleProviders) {
    modules.emplace_back(
        entry.first,
        [moduleName = entry.first, moduleProvider = entry.second]() {
          winrt::Microsoft::ReactNative::Bridge::IReactModuleBuilder
              moduleBuilder = winrt::make<
                  winrt::Microsoft::ReactNative::Bridge::ReactModuleBuilder>();
          moduleBuilder.SetName(winrt::to_hstring(moduleName));
          auto providedModule = moduleProvider(moduleBuilder);
          return moduleBuilder
              .as<winrt::Microsoft::ReactNative::Bridge::ReactModuleBuilder>()
              ->MakeCxxModule(providedModule);
        },
        m_modulesWorkerQueue);
  }

  return modules;
}

NativeModulesProvider::NativeModulesProvider() noexcept {}

void NativeModulesProvider::RegisterModule(
    winrt::Microsoft::ReactNative::Bridge::INativeModule const &module) {
  // TODO: This is taking a naive approach right now and just adding
  // everything. Consider whether to add the CanOverrideExistingModule on
  // INativeModule and then check it here to see whether a module being
  // registered is allowed to take precedence over one that was already
  // registered.
  m_modules.push_back(module);
}

void NativeModulesProvider::AddModuleProvider(
    winrt::hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  m_moduleProviders.emplace(to_string(moduleName), moduleProvider);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
