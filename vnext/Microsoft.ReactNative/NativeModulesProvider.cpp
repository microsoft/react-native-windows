// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulesProvider.h"
#include "ABICxxModule.h"
#include "IReactModuleBuilder.h"

#include <ReactUWP/ReactUwp.h>
#include <folly/json.h>

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::Bridge {
/*-------------------------------------------------------------------------------
  NativeModulesProvider::GetModules
-------------------------------------------------------------------------------*/
std::vector<facebook::react::NativeModuleDescription> NativeModulesProvider::GetModules(
    std::shared_ptr<facebook::react::MessageQueueThread> const & /*defaultQueueThread*/) {
  // std::shared_ptr<facebook::react::MessageQueueThread>
  // queueThread(defaultQueueThread);
  std::vector<facebook::react::NativeModuleDescription> modules;

  if (m_modulesWorkerQueue == nullptr) {
    // TODO: The queue provided is the UIMessageQueueThread which isn't needed
    // for native modules. As a workaround for now let's just use a new worker
    // message queue.
    m_modulesWorkerQueue = react::uwp::CreateWorkerMessageQueue();
  }

  for (auto &entry : m_moduleProviders) {
    modules.emplace_back(
        entry.first,
        [moduleName = entry.first, moduleProvider = entry.second]() {
          IReactModuleBuilder moduleBuilder = winrt::make<ReactModuleBuilder>();
          auto providedModule = moduleProvider(moduleBuilder);
          return moduleBuilder.as<ReactModuleBuilder>()->MakeCxxModule(moduleName, providedModule);
        },
        m_modulesWorkerQueue);
  }

  return modules;
}

NativeModulesProvider::NativeModulesProvider() noexcept {}

void NativeModulesProvider::AddModuleProvider(
    winrt::hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  m_moduleProviders.emplace(to_string(moduleName), moduleProvider);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
