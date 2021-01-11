// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class PopupViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  PopupViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  ShadowNode *createShadow() const override;

  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class PopupShadowNode;
};

} // namespace Microsoft::ReactNative
