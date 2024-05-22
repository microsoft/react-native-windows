// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "SourceCode.h"

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::string>> &ScriptUrlPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::string>> prop{
      L"ReactNative.SourceCode", L"ScriptUrl"};
  return prop;
}

void SourceCode::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
}

ReactNativeSpecs::SourceCodeSpec_SourceCodeConstants SourceCode::GetConstants() noexcept {
  ReactNativeSpecs::SourceCodeSpec_SourceCodeConstants constants;
  constants.scriptURL = *m_reactContext.Properties().Get(ScriptUrlPropertyId());
  return constants;
}

void SourceCode::SetScriptUrl(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &propertyBag,
    const std::string &scriptUrl) noexcept {
  propertyBag.Set(ScriptUrlPropertyId(), scriptUrl);
}

} // namespace Microsoft::ReactNative
