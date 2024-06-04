// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativePlatformConstantsWindowsSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(PlatformConstants)
struct PlatformConstants {
  using ModuleSpec = ReactNativeSpecs::PlatformConstantsWindowsSpec;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::PlatformConstantsWindowsSpec_PlatformConstantsWindows GetConstants() noexcept;
};

} // namespace Microsoft::ReactNative
