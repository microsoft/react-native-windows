// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageBuilder.h"

#include <Fabric/AbiViewComponentDescriptor.h>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// ReactPackageBuilder implementation
//===========================================================================

ReactPackageBuilder::ReactPackageBuilder(
    std::shared_ptr<NativeModulesProvider> const &modulesProvider,
    std::shared_ptr<TurboModulesProvider> const &turboModulesProvider,
    std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> const &componentRegistry,
    std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> const &uriImageManager
    ) noexcept
    : m_modulesProvider{modulesProvider},
      m_turboModulesProvider{turboModulesProvider},
      m_componentRegistry{componentRegistry},
      m_uriImageManager{uriImageManager}
{
}

void ReactPackageBuilder::AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept {
  m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
}

void ReactPackageBuilder::AddTurboModule(
    hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  m_turboModulesProvider->AddModuleProvider(moduleName, moduleProvider, true);
}

void ReactPackageBuilder::AddViewComponent(
    winrt::hstring componentName,
    ReactViewComponentProvider const &viewComponentProvider) noexcept {
  m_componentRegistry->Add(componentName, viewComponentProvider);
}

void ReactPackageBuilder::AddUriImageProvider(
    const winrt::Microsoft::ReactNative::Composition::IUriImageProvider &provider) noexcept {
  m_uriImageManager->AddUriImageProvider(provider);
}

} // namespace winrt::Microsoft::ReactNative
