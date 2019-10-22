// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageBuilder.h"

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// ReactPackageBuilder implementation
//===========================================================================

ReactPackageBuilder::ReactPackageBuilder(
    std::shared_ptr<NativeModulesProvider> const &modulesProvider,
    std::shared_ptr<ViewManagersProvider> const &viewManagersProvider) noexcept
    : m_modulesProvider{modulesProvider},
      m_viewManagersProvider{viewManagersProvider} {}

void ReactPackageBuilder::AddModule(
    hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
}

void ReactPackageBuilder::AddViewManager(
    hstring const &viewManagerName,
    ReactViewManagerProvider const &viewManagerProvider) noexcept {
  m_viewManagersProvider->AddViewManagerProvider(
      viewManagerName, viewManagerProvider);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
