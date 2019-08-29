#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
  ReactNativeHost();

  UIElement GetOrCreateRootView(IInspectable initialProps);

  Microsoft::ReactNative::ReactInstanceManager ReactInstanceManager();
  Microsoft::ReactNative::ReactInstanceSettings InstanceSettings();

  void OnSuspend();
  void OnEnteredBackground();
  void OnLeavingBackground();
  void OnResume(OnResumeAction const &action);

  virtual hstring MainComponentName();
  virtual hstring JavaScriptMainModuleName();
  virtual hstring JavaScriptBundleFile();
  virtual bool UseDeveloperSupport();
  virtual IVectorView<IReactPackage> Packages();

  bool HasInstance() {
    return m_reactInstanceManager != nullptr;
  }

  hstring get_MainComponentName() {
    return overridable().MainComponentName();
  }
  hstring get_JavaScriptMainModuleName() {
    return overridable().JavaScriptMainModuleName();
  }
  hstring get_JavaScriptBundleFile() {
    return overridable().JavaScriptBundleFile();
  }
  bool get_UseDeveloperSupport() {
    return overridable().UseDeveloperSupport();
  }

  IVectorView<IReactPackage> get_Packages() {
    return overridable().Packages();
  }

 private:
  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings;
  Microsoft::ReactNative::ReactInstanceManager m_reactInstanceManager{nullptr};
  hstring m_mainComponentName{};
  hstring m_javascriptMainModuleName{};

  std::shared_ptr<ReactRootView> m_reactRootView{nullptr};

  void Init();
  Microsoft::ReactNative::ReactInstanceManager CreateReactInstanceManager();
  std::shared_ptr<ReactRootView> CreateRootView();
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeHost
    : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
