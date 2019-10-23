// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "ReactNativeHost.g.h"

#include "ReactInstanceManager.h"
#include "ReactInstanceSettings.h"
#include "ReactRootView.h"

#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/ReactUwp.h>

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation::Collections;

#ifndef REACT_DEFAULT_USE_DEVELOPER_SUPPORT
#if _DEBUG
#define REACT_DEFAULT_USE_DEVELOPER_SUPPORT true
#else
#define REACT_DEFAULT_USE_DEVELOPER_SUPPORT false
#endif // _DEBUG
#endif // REACT_DEFAULT_USE_DEVELOPER_SUPPORT

#ifndef REACT_DEFAULT_ENABLE_DEVELOPER_MENU
#if _DEBUG
#define REACT_DEFAULT_ENABLE_DEVELOPER_MENU true
#else
#define REACT_DEFAULT_ENABLE_DEVELOPER_MENU false
#endif // _DEBUG
#endif // REACT_DEFAULT_ENABLE_DEVELOPER_MENU

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
  ReactNativeHost() noexcept;

  UIElement GetOrCreateRootView(IInspectable initialProps) noexcept;

  Microsoft::ReactNative::ReactInstanceManager ReactInstanceManager() noexcept;
  Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(Microsoft::ReactNative::ReactInstanceSettings const &value) noexcept;
  bool HasInstance() noexcept;
  IVector<IReactPackageProvider> PackageProviders() noexcept;
  void PackageProviders(IVector<IReactPackageProvider> const &value) noexcept;
  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;
  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;
  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;
  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  void OnSuspend() noexcept;
  void OnEnteredBackground() noexcept;
  void OnLeavingBackground() noexcept;
  void OnResume(OnResumeAction const &action) noexcept;

 private:
  void Init() noexcept;
  Microsoft::ReactNative::ReactInstanceManager CreateReactInstanceManager() noexcept;
  std::shared_ptr<ReactRootView> CreateRootView() noexcept;

 private:
  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings;
  Microsoft::ReactNative::ReactInstanceManager m_reactInstanceManager{nullptr};
  IVector<IReactPackageProvider> m_packageProviders{nullptr};
  hstring m_mainComponentName;
  bool m_useDeveloperSupport{REACT_DEFAULT_USE_DEVELOPER_SUPPORT};
  hstring m_javaScriptMainModuleName;
  hstring m_javaScriptBundleFile;

  std::shared_ptr<ReactRootView> m_reactRootView{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================
// ReactNativeHost inline implementation
//=============================================================================

inline void ReactNativeHost::InstanceSettings(Microsoft::ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

inline bool ReactNativeHost::HasInstance() noexcept {
  return m_reactInstanceManager != nullptr;
}

inline void ReactNativeHost::PackageProviders(IVector<IReactPackageProvider> const &value) noexcept {
  m_packageProviders = value;
}

inline hstring ReactNativeHost::MainComponentName() noexcept {
  return m_mainComponentName;
}

inline void ReactNativeHost::MainComponentName(hstring const &value) noexcept {
  m_mainComponentName = value;
}

inline bool ReactNativeHost::UseDeveloperSupport() noexcept {
  return m_useDeveloperSupport;
}

inline void ReactNativeHost::UseDeveloperSupport(bool value) noexcept {
  m_useDeveloperSupport = value;
}

inline hstring ReactNativeHost::JavaScriptMainModuleName() noexcept {
  return m_javaScriptMainModuleName;
}

inline void ReactNativeHost::JavaScriptMainModuleName(hstring const &value) noexcept {
  m_javaScriptMainModuleName = value;
}

inline hstring ReactNativeHost::JavaScriptBundleFile() noexcept {
  return m_javaScriptBundleFile;
}

inline void ReactNativeHost::JavaScriptBundleFile(hstring const &value) noexcept {
  m_javaScriptBundleFile = value;
}

} // namespace winrt::Microsoft::ReactNative::implementation
