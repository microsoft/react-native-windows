// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class PickerViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  PickerViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  ShadowNode *createShadow() const override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;

  friend class PickerShadowNode;
};

} // namespace Microsoft::ReactNative
