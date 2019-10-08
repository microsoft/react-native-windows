#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "MainReactNativeHost.g.h"
#include "NativeModulePackage.h"
#include "winrt/SampleLibraryCPP.h"
#include "winrt/SampleLibraryCS.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace SampleLibraryCPP;

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
    return true;
  };

  IVectorView<IReactPackage> Packages() {
    auto packages =
        single_threaded_vector<IReactPackage>({SampleLibraryPackage()});
    return packages.GetView();
  };

  IVectorView<IReactPackageProvider> PackageProviders() {
    OutputDebugStringW(L"My output string.");
    auto packages = single_threaded_vector<IReactPackageProvider>(
        {make<NativeModulePackage>(),
         winrt::SampleLibraryCPP::SampleLibraryCppPackage(),
         winrt::SampleLibraryCS::CsStringsPackageProvider()});
    return packages.GetView();
  };
};
} // namespace winrt::SampleApp::implementation

namespace winrt::SampleApp::factory_implementation {
struct MainReactNativeHost : MainReactNativeHostT<
                                 MainReactNativeHost,
                                 implementation::MainReactNativeHost> {};
} // namespace winrt::SampleApp::factory_implementation
