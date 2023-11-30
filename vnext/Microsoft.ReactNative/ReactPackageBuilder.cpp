// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageBuilder.h"

#ifdef USE_FABRIC
#include <Fabric/AbiViewComponentDescriptor.h>
#endif

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// ReactPackageBuilder implementation
//===========================================================================

ReactPackageBuilder::ReactPackageBuilder(
    std::shared_ptr<NativeModulesProvider> const &modulesProvider,
#ifndef CORE_ABI
    std::shared_ptr<ViewManagersProvider> const &viewManagersProvider,
#endif
    std::shared_ptr<TurboModulesProvider> const &turboModulesProvider,
#ifdef USE_FABRIC
    std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> const &componentRegistry,
#endif
    bool isWebDebugging) noexcept
    : m_modulesProvider{modulesProvider},
#ifndef CORE_ABI
      m_viewManagersProvider{viewManagersProvider},
#endif
      m_turboModulesProvider{turboModulesProvider},
#ifdef USE_FABRIC
      m_componentRegistry{componentRegistry},
#endif

      m_isWebDebugging{isWebDebugging} {
}

void ReactPackageBuilder::AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept {
  m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
}

#ifndef CORE_ABI
void ReactPackageBuilder::AddViewManager(
    hstring const &viewManagerName,
    ReactViewManagerProvider const &viewManagerProvider) noexcept {
  m_viewManagersProvider->AddViewManagerProvider(viewManagerName, viewManagerProvider);
}
#endif

void ReactPackageBuilder::AddTurboModule(
    hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  if (m_isWebDebugging)
    m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
  else
    m_turboModulesProvider->AddModuleProvider(moduleName, moduleProvider, true);
}

#ifdef USE_FABRIC
void ReactPackageBuilder::AddViewComponent(
    winrt::hstring componentName,
    ReactViewComponentProvider const &viewComponentProvider) noexcept {
  m_componentRegistry->Add(componentName, viewComponentProvider);
}
#endif
} // namespace winrt::Microsoft::ReactNative
