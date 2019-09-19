#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SamplePackage.g.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::SampleApp::implementation {
struct SamplePackage : SamplePackageT<SamplePackage> {
  SamplePackage() = default;

  IVectorView<INativeModule> CreateNativeModules(
      ReactContext const &reactContext);
};
} // namespace winrt::SampleApp::implementation

namespace winrt::SampleApp::factory_implementation {
struct SamplePackage
    : SamplePackageT<SamplePackage, implementation::SamplePackage> {};
} // namespace winrt::SampleApp::factory_implementation
