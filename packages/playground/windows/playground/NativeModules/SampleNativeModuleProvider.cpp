// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#include "pch.h"
#include "SampleNativeModuleProvider.h"
#include "HeadlessJsTaskSupport.h"
#include "SampleCxxModule.h"

std::vector<facebook::react::NativeModuleDescription>
SampleNativeModuleProvider::GetModules(
    const std::shared_ptr<facebook::react::MessageQueueThread>
        &defaultQueueThread) {
  // Note The defaultQueueThread is the UI Thread Queue.
  // For maximum app responsive-ness prefer to put modules on a
  // WorkerThreadQueue, unless the UI Thread is required.

  std::vector<facebook::react::NativeModuleDescription> modules;

  modules.emplace_back(
      SampleCxxModule::Name,
      []() { return std::make_unique<SampleCxxModule>(); },
      react::uwp::CreateWorkerMessageQueue());

  modules.emplace_back(
      HeadlessJsTaskSupport::Name,
      []() { return std::make_unique<HeadlessJsTaskSupport>(); },
      react::uwp::CreateWorkerMessageQueue());

  return modules;
}
