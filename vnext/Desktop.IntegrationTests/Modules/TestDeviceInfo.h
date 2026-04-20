// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <codegen/NativeDeviceInfoSpec.g.h>
#include <NativeModules.h>

namespace Microsoft::React::Test {

REACT_MODULE(DeviceInfo)
struct DeviceInfo {
  using ModuleSpec = ::Microsoft::ReactNativeSpecs::DeviceInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants GetConstants() noexcept;
};

} // namespace Microsoft::React::Test
