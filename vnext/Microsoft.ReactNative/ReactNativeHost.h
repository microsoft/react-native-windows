// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactNativeHost.g.h"

#include "NativeModulesProvider.h"
#include "ReactHost/React.h"
#include "ReactInstance.h"
#include "ReactInstanceSettings.h"
#include "ViewManagersProvider.h"

namespace winrt::Microsoft::ReactNative::implementation {

// WinRT ABI-safe implementation of ReactHost.
struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
 public: // ReactNativeHost ABI API
  ReactNativeHost() noexcept;

  // property PackageProviders
  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept;

  // property InstanceSettings
  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  void ReloadInstance() noexcept;

  void OnSuspend() noexcept;
  void OnEnteredBackground() noexcept;
  void OnLeavingBackground() noexcept;
  void OnResume(OnResumeAction const &action) noexcept;
  void OnBackPressed() noexcept;

  void AddLifecycleEventListener(ILifecycleEventListener const& listener);
  void RemoveLifecycleEventListener(ILifecycleEventListener const& listener);

 public:
  Mso::React::IReactHost *ReactHost() noexcept;

 private:
  void RegisterLifecycleEvents();

  Mso::CntPtr<Mso::React::IReactHost> m_reactHost;

  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  ReactNative::ReactInstance m_reactInstance{nullptr};
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders;
  ReactNative::IReactPackageBuilder m_packageBuilder;

  Windows::Foundation::Collections::IVector<ILifecycleEventListener> m_lifecycleEventListeners;
  xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
  xaml::Application::Suspending_revoker m_suspendingRevoker;
  xaml::Application::Resuming_revoker m_resumingRevoker;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
