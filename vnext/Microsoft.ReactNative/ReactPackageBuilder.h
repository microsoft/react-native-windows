#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <Fabric/Composition/UriImageManager.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include "TurboModulesProvider.h"
#include "winrt/Microsoft.ReactNative.Composition.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactPackageBuilder : winrt::implements<ReactPackageBuilder, IReactPackageBuilder, IReactPackageBuilderFabric> {
  ReactPackageBuilder(
      std::shared_ptr<TurboModulesProvider> const &turboModulesProvider,
      std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> const &componentRegistry,
      std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> const
          &uriImageManager) noexcept;

 public: // IReactPackageBuilder
  void AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;
  void AddTurboModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;

  // IReactPackageBuilderFabric
  void AddViewComponent(winrt::hstring componentName, ReactViewComponentProvider const &viewComponentProvider) noexcept;
  void AddUriImageProvider(const winrt::Microsoft::ReactNative::Composition::IUriImageProvider &provider) noexcept;

 private:
  std::shared_ptr<TurboModulesProvider> m_turboModulesProvider;
  std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> m_componentRegistry;
  std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> m_uriImageManager;
};

} // namespace winrt::Microsoft::ReactNative
