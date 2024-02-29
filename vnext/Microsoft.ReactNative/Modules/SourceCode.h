// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeSourceCodeSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(SourceCode)
struct SourceCode {
  using ModuleSpec = ReactNativeSpecs::SourceCodeSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::SourceCodeSpec_SourceCodeConstants GetConstants() noexcept;

  static void SetScriptUrl(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &propertyBag,
      const std::string &scriptUrl) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
};

} // namespace Microsoft::ReactNative
