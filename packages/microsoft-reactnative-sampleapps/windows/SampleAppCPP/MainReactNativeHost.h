#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "MainReactNativeHost.g.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {
struct MainReactNativeHost : MainReactNativeHostT<MainReactNativeHost> {
  MainReactNativeHost() {
    auto instanceSettings = InstanceSettings();
    instanceSettings.UseWebDebugger(
        false); // Disabled temporarily because of issue #2877
    instanceSettings.UseLiveReload(
        true); // true by default in debug builds already
    instanceSettings.UseJsi(true);
  };

  hstring MainComponentName() {
    return L"SampleApp";
  };
  hstring JavaScriptMainModuleName() {
    return L"index.windows";
  };
  bool UseDeveloperSupport() {
    return TRUE;
  };
  IVectorView<IReactPackage> Packages() {
    auto packages = single_threaded_vector<IReactPackage>();
    return packages.GetView();
  };
};
} // namespace winrt::SampleApp::implementation

namespace winrt::SampleApp::factory_implementation {
struct MainReactNativeHost : MainReactNativeHostT<
                                 MainReactNativeHost,
                                 implementation::MainReactNativeHost> {};
} // namespace winrt::SampleApp::factory_implementation
