// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class RefreshControlViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  RefreshControlViewManager(const Mso::React::IReactContext &context);

  ShadowNode *createShadow() const override;

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
};

} // namespace Microsoft::ReactNative
