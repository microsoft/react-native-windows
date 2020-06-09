#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NativeModulesProvider.h"
#include "TurboModulesProvider.h"
#include "ViewManagersProvider.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactPackageBuilder
    : winrt::implements<ReactPackageBuilder, IReactPackageBuilder, IReactPackageBuilderExperimental> {
  ReactPackageBuilder(
      std::shared_ptr<NativeModulesProvider> const &modulesProvider,
      std::shared_ptr<ViewManagersProvider> const &viewManagersProvider,
      std::shared_ptr<TurboModulesProvider> const &turboModulesProvider) noexcept;

 public: // IReactPackageBuilder
  void AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;
  void AddViewManager(hstring const &viewManagerName, ReactViewManagerProvider const &viewManagerProvider) noexcept;
  void AddTurboModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;

 private:
  std::shared_ptr<NativeModulesProvider> m_modulesProvider;
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider;
  std::shared_ptr<TurboModulesProvider> m_turboModulesProvider;
};

} // namespace winrt::Microsoft::ReactNative
