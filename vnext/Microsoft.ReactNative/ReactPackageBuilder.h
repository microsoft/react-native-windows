#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NativeModulesProvider.h"
#include "TurboModulesProvider.h"
#ifndef CORE_ABI
#include "ViewManagersProvider.h"
#endif
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactPackageBuilder : winrt::implements<ReactPackageBuilder, IReactPackageBuilder> {
  ReactPackageBuilder(
      std::shared_ptr<NativeModulesProvider> const &modulesProvider,
#ifndef CORE_ABI
      std::shared_ptr<ViewManagersProvider> const &viewManagersProvider,
#endif
      std::shared_ptr<TurboModulesProvider> const &turboModulesProvider,
      bool isWebDebugging) noexcept;

 public: // IReactPackageBuilder
  void AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;
#ifndef CORE_ABI
  void AddViewManager(hstring const &viewManagerName, ReactViewManagerProvider const &viewManagerProvider) noexcept;
#endif
  void AddTurboModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;

 private:
  std::shared_ptr<NativeModulesProvider> m_modulesProvider;
#ifndef CORE_ABI
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider;
#endif
  std::shared_ptr<TurboModulesProvider> m_turboModulesProvider;
  const bool m_isWebDebugging;
};

} // namespace winrt::Microsoft::ReactNative
