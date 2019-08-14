#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "MainReactNativeHost.g.h"
#include "SamplePackage.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {
struct MainReactNativeHost : MainReactNativeHostT<MainReactNativeHost> {
  MainReactNativeHost() {
    auto instanceSettings = InstanceSettings();
#if DEBUG
    instanceSettings.UseLiveReload(
        true); // true by default in debug builds already
    instanceSettings.UseWebDebugger(
        true); // true by default in debug builds already
#endif
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
    auto packages =
        single_threaded_vector<IReactPackage>({winrt::make<SamplePackage>()});
    return packages.GetView();
  };
};
} // namespace winrt::SampleApp::implementation

namespace winrt::SampleApp::factory_implementation {
struct MainReactNativeHost : MainReactNativeHostT<
                                 MainReactNativeHost,
                                 implementation::MainReactNativeHost> {};
} // namespace winrt::SampleApp::factory_implementation
