// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactApplication.g.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactApplication : ReactApplicationT<ReactApplication> {
  using Super = ReactApplicationT<ReactApplication>;

 public: // ReactApplication ABI API
  ReactApplication() noexcept;

  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept;

  ReactNative::ReactNativeHost Host() noexcept;

  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

 public:
  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &e);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, xaml::Navigation::NavigationFailedEventArgs const &);

 protected:
  virtual ReactApplicationDelegate __stdcall CreateReactApplicationDelegate();

 private:
  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders{nullptr};
  ReactNative::ReactNativeHost m_host{nullptr};

  ReactApplicationDelegate m_delegate{nullptr};

  void OnCreate(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactApplication : ReactApplicationT<ReactApplication, implementation::ReactApplication> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
