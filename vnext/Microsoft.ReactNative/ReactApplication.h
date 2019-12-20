// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactApplication.g.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactApplication : ReactApplicationT<ReactApplication> {
  ReactApplication() noexcept;

  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept;

  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  ReactNative::ReactNativeHost Host() noexcept;

  virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &);
  virtual void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  virtual void OnNavigationFailed(
      IInspectable const &,
      Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

 protected:
  virtual ReactApplicationDelegate __stdcall CreateReactApplicationDelegate();

 private:
  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  Windows::Foundation::Collections::IVector<IReactPackageProvider> m_packageProviders{nullptr};
  hstring m_mainComponentName;
  bool m_useDeveloperSupport{REACT_DEFAULT_USE_DEVELOPER_SUPPORT};
  hstring m_javaScriptMainModuleName;
  hstring m_javaScriptBundleFile;
  ReactNative::ReactNativeHost m_host{nullptr};

  ReactApplicationDelegate m_delegate{nullptr};

  void OnCreate(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &e);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactApplication : ReactApplicationT<ReactApplication, implementation::ReactApplication> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================
// ReactApplication inline implementation
//=============================================================================

inline void ReactApplication::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

inline void ReactApplication::PackageProviders(
    Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept {
  m_packageProviders = value;
}

inline hstring ReactApplication::MainComponentName() noexcept {
  return m_mainComponentName;
}

inline void ReactApplication::MainComponentName(hstring const &value) noexcept {
  m_mainComponentName = value;
}

inline bool ReactApplication::UseDeveloperSupport() noexcept {
  return m_useDeveloperSupport;
}

inline void ReactApplication::UseDeveloperSupport(bool value) noexcept {
  m_useDeveloperSupport = value;
}

inline hstring ReactApplication::JavaScriptMainModuleName() noexcept {
  return m_javaScriptMainModuleName;
}

inline void ReactApplication::JavaScriptMainModuleName(hstring const &value) noexcept {
  m_javaScriptMainModuleName = value;
}

inline hstring ReactApplication::JavaScriptBundleFile() noexcept {
  return m_javaScriptBundleFile;
}

inline void ReactApplication::JavaScriptBundleFile(hstring const &value) noexcept {
  m_javaScriptBundleFile = value;
}

} // namespace winrt::Microsoft::ReactNative::implementation
