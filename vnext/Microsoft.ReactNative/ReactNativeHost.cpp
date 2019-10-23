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

ReactNativeHost::ReactNativeHost() noexcept {
  Init();
}

void ReactNativeHost::Init() noexcept {
#if _DEBUG
  facebook::react::InitializeLogging(
      [](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
        std::string str = std::string("ReactNative:") + message;
        OutputDebugStringA(str.c_str());
      });
#endif
}

Microsoft::ReactNative::ReactInstanceManager
ReactNativeHost::CreateReactInstanceManager() noexcept {
  auto builder = ReactInstanceManagerBuilder();
  builder.InstanceSettings(InstanceSettings());
  builder.UseDeveloperSupport(UseDeveloperSupport());
  builder.InitialLifecycleState(LifecycleState::BeforeCreate);
  builder.JavaScriptBundleFile(JavaScriptBundleFile());
  builder.JavaScriptMainModuleName(JavaScriptMainModuleName());
  builder.PackageProviders(PackageProviders().GetView());

  return builder.Build();
}

std::shared_ptr<ReactRootView> ReactNativeHost::CreateRootView() noexcept {
  auto rootView = std::make_shared<ReactRootView>();
  return rootView;
}

Microsoft::ReactNative::ReactInstanceManager
ReactNativeHost::ReactInstanceManager() noexcept {
  if (m_reactInstanceManager == nullptr) {
    m_reactInstanceManager = CreateReactInstanceManager();
  }

  return m_reactInstanceManager;
}

UIElement ReactNativeHost::GetOrCreateRootView(
    IInspectable initialProps) noexcept {
  if (m_reactRootView != nullptr) {
    return *m_reactRootView;
  }

  folly::dynamic props =
      Microsoft::ReactNative::Bridge::ConvertToDynamic(initialProps);

  m_reactRootView = CreateRootView();
  assert(m_reactRootView != nullptr);

  m_reactRootView->OnCreate(*this);
  m_reactRootView->StartReactApplicationAsync(
      ReactInstanceManager(), MainComponentName(), props);

  return *m_reactRootView;
}

auto ReactNativeHost::InstanceSettings() noexcept
    -> Microsoft::ReactNative::ReactInstanceSettings {
  if (!m_instanceSettings) {
    m_instanceSettings = make<ReactInstanceSettings>();
    m_instanceSettings.UseWebDebugger(false);
    m_instanceSettings.UseLiveReload(true);
    m_instanceSettings.UseJsi(true);
    m_instanceSettings.EnableDeveloperMenu(REACT_DEFAULT_ENABLE_DEVELOPER_MENU);
  }

  return m_instanceSettings;
}

auto ReactNativeHost::PackageProviders() noexcept
    -> IVector<IReactPackageProvider> {
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<IReactPackageProvider>();
  }

  return m_packageProviders;
}

void ReactNativeHost::OnSuspend() noexcept {
  if (HasInstance()) {
    ReactInstanceManager().OnSuspend();
  }
}

void ReactNativeHost::OnEnteredBackground() noexcept {
  if (HasInstance()) {
    ReactInstanceManager().OnEnteredBackground();
  }
}

void ReactNativeHost::OnLeavingBackground() noexcept {
  if (HasInstance()) {
    ReactInstanceManager().OnLeavingBackground();
  }
}

void ReactNativeHost::OnResume(
    Microsoft::ReactNative::OnResumeAction const &action) noexcept {
  if (HasInstance()) {
    ReactInstanceManager().OnResume(action);
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
