// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SamplePackage.h"
#include "SamplePackage.g.cpp"

#include "SampleModule.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {
IVectorView<Bridge::INativeModule> SamplePackage::CreateNativeModules(
    ReactContext const &reactContext) {
  auto modules =
      single_threaded_vector<Bridge::INativeModule>({make<SampleModule>()});

  return modules.GetView();
}

IVectorView<Bridge::IViewManager> SamplePackage::CreateViewManagers(
    ReactContext const &reactContext) {
  auto viewManagers =
      single_threaded_vector<Bridge::IViewManager>();

  return viewManagers.GetView();
}
} // namespace winrt::SampleApp::implementation
