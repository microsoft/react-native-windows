// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace Microsoft::ReactNative {

class CheckBoxViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  CheckBoxViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;


  ShadowNode *createShadow() const override;
  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

 /* void DispatchCommand(const XamlView &viewToUpdate, const std::string &commandId, const folly::dynamic &commandArgs)
      override;*/

  XamlView CreateViewCore(int64_t tag) override;

  friend class CheckBoxShadowNode;
};

} // namespace react::uwp
