// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulesProvider.h"
#include "ABIModule.h"

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

  for (auto module : m_modules) {
    modules.emplace_back(
        winrt::to_string(module.Name()),
        [module]() { return std::make_unique<ABIModule>(module); },
        m_modulesWorkerQueue);
  }

  return modules;
}

void NativeModulesProvider::RegisterModule(
    winrt::Microsoft::ReactNative::Bridge::INativeModule const &module) {
  // TODO: This is taking a naive approach right now and just adding
  // everything. Consider whether to add the CanOverrideExistingModule on
  // INativeModule and then check it here to see whether a module being
  // registered is allowed to take precedence over one that was already
  // registered.
  m_modules.push_back(module);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
