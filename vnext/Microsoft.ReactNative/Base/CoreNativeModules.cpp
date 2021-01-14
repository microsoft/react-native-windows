// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CoreNativeModules.h"

// Modules
#include <AsyncStorageModule.h>
#include <Modules/Animated/NativeAnimatedModule.h>
#include <Modules/AppThemeModuleUwp.h>
#include <Modules/AppearanceModule.h>
#include <Modules/AsyncStorageModuleWin32.h>
#include <Modules/ClipboardModule.h>
#include <Modules/ImageViewManagerModule.h>
#include <Modules/LinkingManagerModule.h>
#include <Modules/NativeUIManager.h>
#include <Modules/NetworkingModule.h>
#include <Modules/PaperUIManagerModule.h>
#include <Threading/MessageQueueThreadFactory.h>

// Shared
#include <CreateModules.h>

namespace react::uwp {

namespace {

bool HasPackageIdentity() noexcept {
  static const bool hasPackageIdentity = []() noexcept {
    auto packageStatics = winrt::get_activation_factory<winrt::Windows::ApplicationModel::IPackageStatics>(
        winrt::name_of<winrt::Windows::ApplicationModel::Package>());
    auto abiPackageStatics = static_cast<winrt::impl::abi_t<winrt::Windows::ApplicationModel::IPackageStatics> *>(
        winrt::get_abi(packageStatics));
    winrt::com_ptr<winrt::impl::abi_t<winrt::Windows::ApplicationModel::IPackage>> dummy;
    return abiPackageStatics->get_Current(winrt::put_abi(dummy)) !=
        winrt::impl::hresult_from_win32(APPMODEL_ERROR_NO_PACKAGE);
  }();

  return hasPackageIdentity;
}

} // namespace

std::vector<facebook::react::NativeModuleDescription> GetCoreModules(
    const std::shared_ptr<facebook::react::MessageQueueThread> &batchingUIMessageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread>
        &jsMessageQueue, // JS engine thread (what we use for external modules)
    std::shared_ptr<react::uwp::AppTheme> &&appTheme,
    Mso::CntPtr<AppearanceChangeListener> &&appearanceListener,
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept {
  std::vector<facebook::react::NativeModuleDescription> modules;

  modules.emplace_back(
      "WebSocketModule",
      [context]() { return Microsoft::React::CreateWebSocketModule(Mso::CntPtr<Mso::React::IReactContext>(context)); },
      jsMessageQueue);

  modules.emplace_back(
      Microsoft::React::NetworkingModule::Name,
      []() { return std::make_unique<Microsoft::React::NetworkingModule>(); },
      jsMessageQueue);

  modules.emplace_back(
      "Timing",
      [batchingUIMessageQueue]() { return facebook::react::CreateTimingModule(batchingUIMessageQueue); },
      batchingUIMessageQueue);

  modules.emplace_back(
      LinkingManagerModule::name, []() { return std::make_unique<LinkingManagerModule>(); }, batchingUIMessageQueue);

  modules.emplace_back(
      ImageViewManagerModule::name,
      []() { return std::make_unique<ImageViewManagerModule>(); },
      batchingUIMessageQueue);

  modules.emplace_back(
      react::uwp::AppThemeModule::Name,
      [appTheme = std::move(appTheme)]() mutable {
        return std::make_unique<react::uwp::AppThemeModule>(std::move(appTheme));
      },
      batchingUIMessageQueue);

  modules.emplace_back(
      NativeAnimatedModule::name,
      [context = std::move(context)]() mutable { return std::make_unique<NativeAnimatedModule>(std::move(context)); },
      batchingUIMessageQueue);

  modules.emplace_back(
      AppearanceModule::Name,
      [appearanceListener = std::move(appearanceListener)]() mutable {
        return std::make_unique<AppearanceModule>(std::move(appearanceListener));
      },
      jsMessageQueue);

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

} // namespace react::uwp
