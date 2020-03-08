// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <InstanceManager.h>

#include <DevSettings.h>
#include <IDevSupportManager.h>
#include <OInstance.h>

#include <cxxreact/Instance.h>

namespace facebook {
namespace react {

namespace {

const std::shared_ptr<IDevSupportManager> &GetSharedDevManager() noexcept {
  static std::shared_ptr<IDevSupportManager> s_devManager(CreateDevSupportManager());

  return s_devManager;
}

} // namespace

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> instance = InstanceImpl::MakeNoBundle(
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(uimanager),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      GetSharedDevManager());

  return instance;
}

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::string &&jsBundleBasePath,
    std::string &&jsBundleRelativePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> instance = InstanceImpl::MakeAndLoadBundle(
      std::move(jsBundleBasePath),
      std::move(jsBundleRelativePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(uimanager),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      GetSharedDevManager());

  return instance;
}

} // namespace react
} // namespace facebook
