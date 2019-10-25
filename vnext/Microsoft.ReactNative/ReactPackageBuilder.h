#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "NativeModulesProvider.h"
#include "ViewManagersProvider.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ReactPackageBuilder : winrt::implements<ReactPackageBuilder, IReactPackageBuilder> {
  ReactPackageBuilder(
      std::shared_ptr<NativeModulesProvider> const &modulesProvider,
      std::shared_ptr<ViewManagersProvider> const &viewManagersProvider) noexcept;

 public: // IReactPackageBuilder
  void AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;
  void AddViewManager(hstring const &viewManagerName, ReactViewManagerProvider const &viewManagerProvider) noexcept;

 private:
  std::shared_ptr<NativeModulesProvider> m_modulesProvider;
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
