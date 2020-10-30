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
#include <Modules/LocationObserverModule.h>
#include <Modules/NativeUIManager.h>
#include <Modules/NetworkingModule.h>
#include <Modules/UIManagerModule.h>
#include <Modules/WebSocketModuleUwp.h>
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
  }
  ();

  return hasPackageIdentity;
}

} // namespace

std::vector<facebook::react::NativeModuleDescription> GetCoreModules(
    const std::shared_ptr<facebook::react::IUIManager> &uiManager,
    const std::shared_ptr<facebook::react::MessageQueueThread> &messageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread> &uiMessageQueue,
    std::shared_ptr<react::uwp::AppTheme> &&appTheme,
    Mso::CntPtr<AppearanceChangeListener> &&appearanceListener,
    const std::shared_ptr<IReactInstance> &uwpInstance) noexcept {
  // Modules
  std::vector<facebook::react::NativeModuleDescription> modules;

  modules.emplace_back(
      "UIManager",
      [uiManager, uiMessageQueue]() {
        return facebook::react::createUIManagerModule(std::shared_ptr(uiManager), std::shared_ptr(uiMessageQueue));
      },
      messageQueue);

  modules.emplace_back(
      react::uwp::WebSocketModule::Name,
      []() { return std::make_unique<react::uwp::WebSocketModule>(); },
      MakeSerialQueueThread());

  modules.emplace_back(
      NetworkingModule::Name, []() { return std::make_unique<NetworkingModule>(); }, MakeSerialQueueThread());

  modules.emplace_back(
      "Timing", [messageQueue]() { return facebook::react::CreateTimingModule(messageQueue); }, messageQueue);

  modules.emplace_back(
      LinkingManagerModule::name, []() { return std::make_unique<LinkingManagerModule>(); }, messageQueue);

  modules.emplace_back(
      ImageViewManagerModule::name,
      [messageQueue]() { return std::make_unique<ImageViewManagerModule>(); },
      messageQueue);

  modules.emplace_back(
      LocationObserverModule::name,
      [messageQueue]() { return std::make_unique<LocationObserverModule>(messageQueue); },
      MakeSerialQueueThread()); // TODO: figure out threading

  modules.emplace_back(
      react::uwp::AppThemeModule::Name,
      [appTheme = std::move(appTheme)]() mutable {
        return std::make_unique<react::uwp::AppThemeModule>(std::move(appTheme));
      },
      messageQueue);

  modules.emplace_back(
      NativeAnimatedModule::name,
      [wpUwpInstance = std::weak_ptr(uwpInstance), uiMessageQueue]() mutable {
        return std::make_unique<NativeAnimatedModule>(std::move(wpUwpInstance), uiMessageQueue);
      },
      messageQueue);

  modules.emplace_back(
      AppearanceModule::Name,
      [appearanceListener = std::move(appearanceListener)]() mutable {
        return std::make_unique<AppearanceModule>(std::move(appearanceListener));
      },
      messageQueue);

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
      MakeSerialQueueThread());

  return modules;
}

} // namespace react::uwp
