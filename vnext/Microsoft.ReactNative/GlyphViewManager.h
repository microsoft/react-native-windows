// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class GlyphViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  GlyphViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  ShadowNode *createShadow() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace Microsoft::ReactNative
