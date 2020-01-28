#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <NativeModuleProvider.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

class NativeModulesProvider final : public facebook::react::NativeModuleProvider {
 public:
  virtual std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread) override;

 public:
  NativeModulesProvider() noexcept;
  void AddModuleProvider(winrt::hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;

 private:
  std::map<std::string, ReactModuleProvider> m_moduleProviders;
  std::shared_ptr<facebook::react::MessageQueueThread> m_modulesWorkerQueue{nullptr};
  IReactPackageBuilder m_packageBuilder;
};

} // namespace winrt::Microsoft::ReactNative
