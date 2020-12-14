// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// public header

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <TurboModuleManager.h>
#include <cxxreact/Instance.h>
#include "InstanceManager.h"

namespace facebook {
namespace react {

struct IDevSupportManager;
struct IReactRootView;

class InstanceImpl final : public InstanceWrapper, private ::std::enable_shared_from_this<InstanceImpl> {
 public:
  static std::shared_ptr<InstanceImpl> MakeNoBundle(
      std::shared_ptr<Instance> &&instance,
      std::string &&jsBundleBasePath,
      std::vector<
          std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
          &&cxxModules,
      std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
      std::unique_ptr<InstanceCallback> &&callback,
      std::shared_ptr<MessageQueueThread> jsQueue,
      std::shared_ptr<MessageQueueThread> nativeQueue,
      std::shared_ptr<DevSettings> devSettings,
      std::shared_ptr<IDevSupportManager> devManager) noexcept;

  static std::shared_ptr<InstanceImpl> MakeAndLoadBundle(
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
      std::shared_ptr<DevSettings> devSettings,
      std::shared_ptr<IDevSupportManager> devManager) noexcept;

  // Instance methods
  void loadBundle(std::string &&jsBundleRelativePath) override;
  void loadBundleSync(std::string &&jsBundleRelativePath) override;
  virtual const std::shared_ptr<Instance> &GetInstance() const noexcept override {
    return m_innerInstance;
  }

  void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) override;
  void invokeCallback(const int64_t callbackId, folly::dynamic &&params) override;

  ~InstanceImpl();

 private:
  InstanceImpl(
      std::shared_ptr<Instance> &&instance,
      std::string &&jsBundleFile,
      std::vector<
          std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
          &&cxxModules,
      std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
      std::unique_ptr<InstanceCallback> &&callback,
      std::shared_ptr<MessageQueueThread> jsQueue,
      std::shared_ptr<MessageQueueThread> nativeQueue,
      std::shared_ptr<DevSettings> devSettings,
      std::shared_ptr<IDevSupportManager> devManager);

  InstanceImpl(
      std::shared_ptr<Instance> &&instance,
      std::string &&jsBundleBasePath,
      std::string &&jsBundleRelativePath,
      std::vector<
          std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
          &&cxxModules,
      std::unique_ptr<InstanceCallback> &&callback,
      std::shared_ptr<MessageQueueThread> jsQueue,
      std::shared_ptr<MessageQueueThread> nativeQueue,
      std::shared_ptr<DevSettings> devSettings,
      std::shared_ptr<IDevSupportManager> devManager);

  std::vector<std::unique_ptr<NativeModule>> GetDefaultNativeModules(std::shared_ptr<MessageQueueThread> nativeQueue);
  void RegisterForReloadIfNecessary() noexcept;
  void loadBundleInternal(std::string &&jsBundleRelativePath, bool synchronously);
  void SetInError() noexcept;

 private:
  std::shared_ptr<Instance> m_innerInstance;

  std::string m_jsBundleBasePath;
  std::shared_ptr<facebook::react::ModuleRegistry> m_moduleRegistry;
  std::shared_ptr<TurboModuleRegistry> m_turboModuleRegistry;
  std::shared_ptr<MessageQueueThread> m_jsThread;
  std::shared_ptr<MessageQueueThread> m_nativeQueue;

  std::shared_ptr<IDevSupportManager> m_devManager;
  std::shared_ptr<DevSettings> m_devSettings;
  bool m_isInError{false};
};

} // namespace react
} // namespace facebook
