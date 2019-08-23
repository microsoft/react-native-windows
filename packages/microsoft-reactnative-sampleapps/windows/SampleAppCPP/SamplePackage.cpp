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
IVectorView<Bridge::NativeModuleBase> SamplePackage::CreateNativeModules(
    ReactContext const &reactContext) {
  auto modules =
      single_threaded_vector<Bridge::NativeModuleBase>({make<SampleModule>()});

  return modules.GetView();
}
} // namespace winrt::SampleApp::implementation
