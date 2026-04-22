// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestDeviceInfo.h"

namespace Microsoft::React::Test {

void DeviceInfo::Initialize(winrt::Microsoft::ReactNative::ReactContext const &) noexcept {}

::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants DeviceInfo::GetConstants() noexcept {
  ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants constants;
  ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DisplayMetrics dm;
  dm.fontScale = 1;
  dm.height = 1024;
  dm.width = 1024;
  dm.scale = 1;
  constants.Dimensions.screen = dm;
  constants.Dimensions.window = dm;
  return constants;
}

} // namespace Microsoft::React::Test
