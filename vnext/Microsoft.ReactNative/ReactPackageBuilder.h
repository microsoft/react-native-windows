#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NativeModulesProvider.h"
#include "TurboModulesProvider.h"
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
#include "ViewManagersProvider.h"
#endif
#include "winrt/Microsoft.ReactNative.h"
#ifdef USE_FABRIC
#include <Fabric/Composition/UriImageManager.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include "winrt/Microsoft.ReactNative.Composition.h"
#endif

namespace winrt::Microsoft::ReactNative {

struct ReactPackageBuilder : winrt::implements<
                                 ReactPackageBuilder,
                                 IReactPackageBuilder
#ifdef USE_FABRIC
                                 ,
                                 IReactPackageBuilderFabric
#endif
                                 > {
  ReactPackageBuilder(
      std::shared_ptr<NativeModulesProvider> const &modulesProvider,
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
      std::shared_ptr<ViewManagersProvider> const &viewManagersProvider,
#endif
      std::shared_ptr<TurboModulesProvider> const &turboModulesProvider
#ifdef USE_FABRIC
      ,
      std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> const &componentRegistry,
      std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> const
          &uriImageManager
#endif
      ) noexcept;

 public: // IReactPackageBuilder
  void AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
  void AddViewManager(hstring const &viewManagerName, ReactViewManagerProvider const &viewManagerProvider) noexcept;
#endif
  void AddTurboModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept;

#ifdef USE_FABRIC
  // IReactPackageBuilderFabric
  void AddViewComponent(winrt::hstring componentName, ReactViewComponentProvider const &viewComponentProvider) noexcept;
  void AddUriImageProvider(const winrt::Microsoft::ReactNative::Composition::IUriImageProvider &provider) noexcept;
#endif // USE_FABRIC

 private:
  std::shared_ptr<NativeModulesProvider> m_modulesProvider;
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider;
#endif
  std::shared_ptr<TurboModulesProvider> m_turboModulesProvider;

#ifdef USE_FABRIC
  std::shared_ptr<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry> m_componentRegistry;
  std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> m_uriImageManager;
#endif
};

} // namespace winrt::Microsoft::ReactNative
