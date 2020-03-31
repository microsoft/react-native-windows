// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Logging.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/JSBigString.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "DevSettings.h"
#include "IReactRootView.h"

namespace folly {
struct dynamic;
}

namespace facebook {

namespace react {

class Instance;
class MessageQueueThread;
class ModuleRegistry;
class IUIManager;
class TurboModuleRegistry;

struct InstanceWrapper {
  virtual const std::shared_ptr<Instance> &GetInstance() const noexcept = 0;

  virtual void AttachMeasuredRootView(IReactRootView *rootView, folly::dynamic &&initProps) noexcept = 0;
  virtual void DetachRootView(IReactRootView *rootView) noexcept = 0;

  virtual void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) = 0;
  virtual void invokeCallback(const int64_t callbackId, folly::dynamic &&params) = 0;
  virtual void loadBundle(std::string &&jsBundleRelativePath) = 0;
  virtual void loadBundleSync(std::string &&jsBundleRelativePath) = 0;
};

// Things that used to be exported from InstanceManager, but probably belong
// elsewhere
std::shared_ptr<InstanceWrapper> CreateReactInstance(
    std::string &&jsBundleRelativePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings) noexcept;

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
    std::shared_ptr<DevSettings> devSettings) noexcept;

} // namespace react
} // namespace facebook
