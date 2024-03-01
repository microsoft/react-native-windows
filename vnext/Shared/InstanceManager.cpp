// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <InstanceManager.h>

#include <DevSettings.h>
#include <IDevSupportManager.h>
#include <OInstance.h>

#include <cxxreact/Instance.h>

namespace Microsoft::ReactNative {

const std::shared_ptr<facebook::react::IDevSupportManager> &GetSharedDevManager() noexcept {
  static std::shared_ptr<facebook::react::IDevSupportManager> s_devManager(facebook::react::CreateDevSupportManager());

  return s_devManager;
}
} // namespace Microsoft::ReactNative

namespace facebook {
namespace react {

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> inner = InstanceImpl::MakeNoBundle(
      std::move(instance),
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      nullptr, // longLivedObjectCollection
      nullptr, // PropertyBag
      std::move(callback),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      ::Microsoft::ReactNative::GetSharedDevManager());

  return inner;
}

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
    const winrt::Microsoft::ReactNative::IReactPropertyBag &propertyBag,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> inner = InstanceImpl::MakeNoBundle(
      std::move(instance),
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(longLivedObjectCollection),
      propertyBag,
      std::move(callback),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      ::Microsoft::ReactNative::GetSharedDevManager());

  return inner;
}

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::string &&jsBundleRelativePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept {
  // Now create the instance
  std::shared_ptr<InstanceWrapper> inner = InstanceImpl::MakeAndLoadBundle(
      std::move(instance),
      std::move(jsBundleBasePath),
      std::move(jsBundleRelativePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(callback),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      ::Microsoft::ReactNative::GetSharedDevManager());

  return inner;
}

} // namespace react
} // namespace facebook
