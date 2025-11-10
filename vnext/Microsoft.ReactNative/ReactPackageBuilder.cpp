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
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
    std::shared_ptr<ViewManagersProvider> const &viewManagersProvider,
#endif
    std::shared_ptr<TurboModulesProvider> const &turboModulesProvider
#ifdef USE_FABRIC
    ,
    std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> const &componentRegistry,
    std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> const &uriImageManager
#endif
    ) noexcept
    : m_modulesProvider{modulesProvider},
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
      m_viewManagersProvider{viewManagersProvider},
#endif
      m_turboModulesProvider{turboModulesProvider}
#ifdef USE_FABRIC
,
      m_componentRegistry{componentRegistry},
      m_uriImageManager{uriImageManager}
#endif
 {
}

void ReactPackageBuilder::AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept {
  m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
}

#if !defined(CORE_ABI) && !defined(USE_FABRIC)
void ReactPackageBuilder::AddViewManager(
    hstring const &viewManagerName,
    ReactViewManagerProvider const &viewManagerProvider) noexcept {
  m_viewManagersProvider->AddViewManagerProvider(viewManagerName, viewManagerProvider);
}
#endif

void ReactPackageBuilder::AddTurboModule(
    hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
    m_turboModulesProvider->AddModuleProvider(moduleName, moduleProvider, true);
}

#ifdef USE_FABRIC
void ReactPackageBuilder::AddViewComponent(
    winrt::hstring componentName,
    ReactViewComponentProvider const &viewComponentProvider) noexcept {
  m_componentRegistry->Add(componentName, viewComponentProvider);
}

void ReactPackageBuilder::AddUriImageProvider(
    const winrt::Microsoft::ReactNative::Composition::IUriImageProvider &provider) noexcept {
  m_uriImageManager->AddUriImageProvider(provider);
}

#endif
} // namespace winrt::Microsoft::ReactNative
