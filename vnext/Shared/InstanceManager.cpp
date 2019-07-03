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
  static std::shared_ptr<IDevSupportManager> s_devManager(
      CreateDevSupportManager());

  return s_devManager;
}

} // namespace

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::string &&jsBundleBasePath,
    std::vector<std::tuple<
        std::string,
        facebook::xplat::module::CxxModule::Provider,
        std::shared_ptr<MessageQueueThread>>> &&cxxModules,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> instance = InstanceImpl::MakeNoBundle(
      std::move(jsBundleBasePath),
      std::move(cxxModules),
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
    std::vector<std::tuple<
        std::string,
        facebook::xplat::module::CxxModule::Provider,
        std::shared_ptr<MessageQueueThread>>> &&cxxModules,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> instance = InstanceImpl::MakeAndLoadBundle(
      std::move(jsBundleBasePath),
      std::move(jsBundleRelativePath),
      std::move(cxxModules),
      std::move(uimanager),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      GetSharedDevManager());

  return instance;
}

#if (defined(_MSC_VER) && !defined(WINRT))
#if !defined(OSS_RN)
std::shared_ptr<InstanceWrapper> CreateReactInstanceForSandbox(
    std::string &&jsString,
    std::string &&configsString,
    std::string &&sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::function<void(std::string &&message)>
        &&sendNativeModuleCall) noexcept {
  return CreateReactInstanceForSandbox(
      std::string(),
      std::move(jsString),
      std::move(configsString),
      std::move(sourceUrl),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(sendNativeModuleCall));
};

std::shared_ptr<InstanceWrapper> CreateReactInstanceForSandbox(
    std::string &&jsBundleBasePath,
    std::string &&jsBundleRelativePath,
    std::string &&configsString,
    std::string &&sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::function<void(std::string &&message)>
        &&sendNativeModuleCall) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> instance = InstanceImpl::MakeSandbox(
      std::move(jsBundleBasePath),
      std::move(jsBundleRelativePath),
      std::move(configsString),
      std::move(sourceUrl),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      GetSharedDevManager(),
      std::move(sendNativeModuleCall));

  return instance;
}
#endif
#endif

} // namespace react
} // namespace facebook
