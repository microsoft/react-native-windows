// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativePlatformConstantsWinSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(PlatformConstants)
struct PlatformConstants {
  using ModuleSpec = ReactNativeSpecs::PlatformConstantsWinSpec;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::PlatformConstantsWinSpec_PlatformConstantsWindows GetConstants() noexcept;
};

} // namespace Microsoft::ReactNative
