// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Module.h>
#include <NativeModuleProvider.h>
#include <combaseapi.h>
#include <cxxreact/CxxModule.h>
#include <wrl.h>

namespace ABI {
namespace react {
namespace uwp {

class ABIModule;

class ABIModule : public facebook::xplat::module::CxxModule {
 public:
  ABIModule(const Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> &module);

  std::string getName() override;

  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;

 private:
  Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> m_module;
};

class ABIModuleLoader : public facebook::react::NativeModuleProvider {
 public:
  ABIModuleLoader();

  void RegisterModule(Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> &module);

  // NativeModuleProvider
  std::vector<facebook::react::NativeModuleDescription> GetModules(
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread) override;

 private:
  std::vector<Microsoft::WRL::ComPtr<ABI::react::uwp::IModule>> m_modules;
};

} // namespace uwp
} // namespace react
} // namespace ABI
