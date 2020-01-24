// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactNativeHost.g.h"

#include "NativeModulesProvider.h"
#include "ReactInstanceSettings.h"
#include "ReactRootView2.h"
#include "ViewManagersProvider.h"

#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/ReactUwp.h>

#include <ReactHost/React.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
 public: // ReactNativeHost ABI API
  ReactNativeHost() noexcept;

  ReactNative::IReactContext CurrentReactContext() noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept;

  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::IAsyncAction ReloadInstance() noexcept;

  void OnSuspend() noexcept;
  void OnEnteredBackground() noexcept;
  void OnLeavingBackground() noexcept;
  void OnResume(OnResumeAction const &action) noexcept;
  void OnBackPressed() noexcept;

 public:
  std::shared_ptr<react::uwp::IReactInstance> Instance() noexcept;

 private:
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders{nullptr};
  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};

  IReactContext m_currentReactContext{nullptr};
  std::shared_ptr<NativeModulesProvider> m_modulesProvider{nullptr};
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider{nullptr};
  IReactPackageBuilder m_packageBuilder;

  Mso::CntPtr<Mso::React::IReactHost> m_reactHost;
  std::shared_ptr<react::uwp::IReactInstance> m_instance;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
