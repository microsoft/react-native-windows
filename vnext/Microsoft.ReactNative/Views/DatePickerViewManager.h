// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class DatePickerViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  DatePickerViewManager(const Mso::React::IReactContext &context);

  ShadowNode *createShadow() const;
  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;

  friend class DatePickerShadowNode;
};

} // namespace Microsoft::ReactNative
