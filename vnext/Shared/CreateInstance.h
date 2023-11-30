// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>
#include <winrt/Microsoft.ReactNative.h>

namespace facebook::react {

std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleRelativePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
    const winrt::Microsoft::ReactNative::IReactPropertyBag &propertyBag,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept;

} // namespace facebook::react
