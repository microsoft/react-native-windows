// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CoreNativeModules.h"

// Modules
#include <AppModelHelpers.h>
#include <AsyncStorageModule.h>
#include <Modules/AsyncStorageModuleWin32.h>
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

  // AsyncStorageModule doesn't work without package identity (it indirectly depends on
  // Windows.Storage.StorageFile), so check for package identity before adding it.
  modules.emplace_back(
      "AsyncLocalStorage",
      []() -> std::unique_ptr<facebook::xplat::module::CxxModule> {
        if (HasPackageIdentity()) {
          return std::make_unique<facebook::react::AsyncStorageModule>(L"asyncStorage");
        } else {
          return std::make_unique<facebook::react::AsyncStorageModuleWin32>();
        }
      },
      jsMessageQueue);

  return modules;
}

} // namespace Microsoft::ReactNative
