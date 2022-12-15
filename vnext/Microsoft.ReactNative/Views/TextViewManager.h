// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>
#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class TextViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  TextViewManager(const Mso::React::IReactContext &context);

  ShadowNode *createShadow() const override;

  const wchar_t *GetName() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;
  void UpdateProperties(ShadowNodeBase *nodeToUpdate, winrt::Microsoft::ReactNative::JSValueObject &props) override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

  void OnPointerEvent(ShadowNodeBase *node, const winrt::Microsoft::ReactNative::ReactPointerEventArgs &args) override;

  static void UpdateTextHighlighters(ShadowNodeBase *node, bool highlightAdded);

  static void UpdateOptimizedText(ShadowNodeBase *node);

  static void SetDescendantPressable(ShadowNodeBase *node);

  static TextTransform GetTextTransformValue(ShadowNodeBase *node);

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;
};

} // namespace Microsoft::ReactNative
