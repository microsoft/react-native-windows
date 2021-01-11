// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulesProvider.h"
#include "ABICxxModule.h"
#include "IReactContext.h"
#include "IReactModuleBuilder.h"
#include "Threading/MessageQueueThreadFactory.h"

#include <folly/json.h>

#include "ReactHost/MsoUtils.h"

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative {
/*-------------------------------------------------------------------------------
  NativeModulesProvider::GetModules
-------------------------------------------------------------------------------*/
std::vector<facebook::react::NativeModuleDescription> NativeModulesProvider::GetModules(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    std::shared_ptr<facebook::react::MessageQueueThread> const &defaultQueueThread) {
  std::vector<facebook::react::NativeModuleDescription> modules;

<<<<<<< HEAD
  if (m_modulesWorkerQueue == nullptr) {
    // TODO: The queue provided is the UIMessageQueueThread which isn't needed
    // for native modules. As a workaround for now let's just use a new worker
    // message queue.
    m_modulesWorkerQueue = react::uwp::MakeSerialQueueThread();
  }

  auto winrtReactContext = winrt::make<implementation::ReactContext>(Mso::Copy(reactContext));
||||||| 811c767bf
  if (m_modulesWorkerQueue == nullptr) {
    // TODO: The queue provided is the UIMessageQueueThread which isn't needed
    // for native modules. As a workaround for now let's just use a new worker
    // message queue.
    m_modulesWorkerQueue = react::uwp::MakeSerialQueueThread();
  }

  auto winrtReactContext = winrt::make<ReactContext>(Mso::Copy(reactContext));
=======
  auto winrtReactContext = winrt::make<implementation::ReactContext>(Mso::Copy(reactContext));
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

  for (auto &entry : m_moduleProviders) {
    modules.emplace_back(
        entry.first,
        [ moduleName = entry.first, moduleProvider = entry.second, winrtReactContext ]() noexcept {
          IReactModuleBuilder moduleBuilder = winrt::make<ReactModuleBuilder>(winrtReactContext);
          auto providedModule = moduleProvider(moduleBuilder);
          return moduleBuilder.as<ReactModuleBuilder>()->MakeCxxModule(moduleName, providedModule);
        },
        defaultQueueThread);
  }

  return modules;
}

void NativeModulesProvider::AddModuleProvider(
    winrt::hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  m_moduleProviders.emplace(to_string(moduleName), moduleProvider);
}

} // namespace winrt::Microsoft::ReactNative
