// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CoreNativeModules.h"

// Modules
#include <AppModelHelpers.h>
#include <Modules/ClipboardModule.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Threading/MessageQueueThreadFactory.h>

// Shared
#include <CreateModules.h>

namespace Microsoft::ReactNative {

std::vector<facebook::react::NativeModuleDescription> GetCoreModules(
    const std::shared_ptr<facebook::react::MessageQueueThread> &batchingUIMessageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread>
        &jsMessageQueue, // JS engine thread (what we use for external modules)
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept {
  std::vector<facebook::react::NativeModuleDescription> modules;

  modules.emplace_back(
      "Networking",
      [props = context->Properties()]() { return Microsoft::React::CreateHttpModule(props); },
      jsMessageQueue);

  modules.emplace_back(
      Microsoft::React::GetBlobModuleName(),
      [props = context->Properties()]() { return Microsoft::React::CreateBlobModule(props); },
      batchingUIMessageQueue);

  modules.emplace_back(
      Microsoft::React::GetFileReaderModuleName(),
      [props = context->Properties()]() { return Microsoft::React::CreateFileReaderModule(props); },
      batchingUIMessageQueue);

  return modules;
}

} // namespace Microsoft::ReactNative
