// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class TextInputViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  TextInputViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;
  virtual void TransferProperties(const XamlView &oldView, const XamlView &newView) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class TextInputShadowNode;

 private:
  void TransferInputScope(const XamlView &oldView, const XamlView &newView, const bool copyToPasswordBox);
};

} // namespace Microsoft::ReactNative
