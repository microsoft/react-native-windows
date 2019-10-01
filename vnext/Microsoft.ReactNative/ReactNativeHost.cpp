// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactInstanceManager.h"
#include "ReactInstanceManagerBuilder.h"
#include "ReactInstanceSettings.h"
#include "ReactRootView.h"
#include "ReactSupport.h"

#include <NativeModuleProvider.h>
#include <ViewManager.h>
#include <ViewManagerProvider.h>

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeHost::ReactNativeHost() {
  Init();
}

Microsoft::ReactNative::ReactInstanceManager
ReactNativeHost::ReactInstanceManager() {
  if (m_reactInstanceManager == nullptr) {
    m_reactInstanceManager = CreateReactInstanceManager();
  }

  return m_reactInstanceManager;
}

UIElement ReactNativeHost::GetOrCreateRootView(IInspectable initialProps) {
  if (m_reactRootView != nullptr)
    return *m_reactRootView;

  folly::dynamic props =
      Microsoft::ReactNative::Bridge::ConvertToDynamic(initialProps);

  m_reactRootView = CreateRootView();
  assert(m_reactRootView != nullptr);

  m_reactRootView->OnCreate(*this);
  m_reactRootView->StartReactApplicationAsync(
      ReactInstanceManager(), get_MainComponentName(), props);

  return *m_reactRootView;
}

void ReactNativeHost::OnSuspend() {
  if (HasInstance()) {
    ReactInstanceManager().OnSuspend();
  }
}

void ReactNativeHost::OnEnteredBackground() {
  if (HasInstance()) {
    ReactInstanceManager().OnEnteredBackground();
  }
}

void ReactNativeHost::OnLeavingBackground() {
  if (HasInstance()) {
    ReactInstanceManager().OnLeavingBackground();
  }
}

void ReactNativeHost::OnResume(
    Microsoft::ReactNative::OnResumeAction const &action) {
  if (HasInstance()) {
    ReactInstanceManager().OnResume(action);
  }
}

hstring ReactNativeHost::MainComponentName() {
  throw hresult_not_implemented(
      L"Must implement ReactNativeHost.MainComponentName.");
}

hstring ReactNativeHost::JavaScriptMainModuleName() {
  throw hresult_not_implemented(
      L"Must implement ReactNativeHost.JavaScriptMainModuleName.");
}

hstring ReactNativeHost::JavaScriptBundleFile() {
  return {};
}

bool ReactNativeHost::UseDeveloperSupport() {
#if _DEBUG
  return true;
#else
  return false;
#endif
}

auto ReactNativeHost::Packages() -> IVectorView<IReactPackage> {
  return single_threaded_vector<IReactPackage>().GetView();
}

auto ReactNativeHost::ModulePackages() -> IVectorView<INativeModulePackage> {
  return single_threaded_vector<INativeModulePackage>().GetView();
}

auto ReactNativeHost::InstanceSettings()
    -> Microsoft::ReactNative::ReactInstanceSettings {
  // Return the default
  if (m_instanceSettings == nullptr)
    m_instanceSettings = make<ReactInstanceSettings>();

  return m_instanceSettings;
}

void ReactNativeHost::Init() {
#if _DEBUG
  facebook::react::InitializeLogging(
      [](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
        std::string str = std::string("ReactNative:") + message;
        OutputDebugStringA(str.c_str());
      });
#endif
}

Microsoft::ReactNative::ReactInstanceManager
ReactNativeHost::CreateReactInstanceManager() {
  auto builder = ReactInstanceManagerBuilder();
  builder.InstanceSettings(InstanceSettings());
  builder.UseDeveloperSupport(UseDeveloperSupport());
  builder.InitialLifecycleState(LifecycleState::BeforeCreate);
  builder.JavaScriptBundleFile(get_JavaScriptBundleFile());
  builder.JavaScriptMainModuleName(get_JavaScriptMainModuleName());
  builder.Packages(get_Packages());
  builder.ModulePackages(get_ModulePackages());

  return builder.Build();
}

std::shared_ptr<ReactRootView> ReactNativeHost::CreateRootView() {
  auto rootView = std::make_shared<ReactRootView>();
  return rootView;
}
} // namespace winrt::Microsoft::ReactNative::implementation
