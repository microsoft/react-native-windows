#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "SamplePackage.g.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::Playground::implementation {
struct SamplePackage : SamplePackageT<SamplePackage> {
  SamplePackage() = default;

  IVectorView<NativeModuleBase> CreateNativeModules(
      ReactContext const &reactContext);
};
} // namespace winrt::Playground::implementation

namespace winrt::Playground::factory_implementation {
struct SamplePackage
    : SamplePackageT<SamplePackage, implementation::SamplePackage> {};
} // namespace winrt::Playground::factory_implementation
