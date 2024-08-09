// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>

#include "codegen/NativeStatusBarManagerAndroidSpec.g.h"

namespace Microsoft::ReactNative {

REACT_MODULE(StatusBarManager)
struct StatusBarManager {
  using ModuleSpec = Microsoft::ReactNativeSpecs::StatusBarManagerAndroidSpec;

  REACT_GET_CONSTANTS(GetConstants)
  static Microsoft::ReactNativeSpecs::StatusBarManagerAndroidSpec_Constants GetConstants() noexcept {
    return {0};
  }

  REACT_METHOD(setColor)
  static void setColor(double /*color*/, bool /*animated*/) noexcept {}

  REACT_METHOD(setTranslucent)
  static void setTranslucent(bool /*translucent*/) noexcept {}

  REACT_METHOD(setStyle)
  static void setStyle(std::optional<std::string> /*statusBarStyle*/) noexcept {}

  REACT_METHOD(setHidden)
  static void setHidden(bool /*hidden*/) noexcept {}
};

} // namespace Microsoft::ReactNative
