// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>
#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

enum class PropertyChangeType : std::uint_fast8_t {
  None = 0,
  Text = 1 << 0,
  AddBackgroundColor = 1 << 1,
};

DEFINE_ENUM_FLAG_OPERATORS(PropertyChangeType);

class TextViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  TextViewManager(const Mso::React::IReactContext &context);

  ShadowNode *createShadow() const override;

  const wchar_t *GetName() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

  void OnDescendantTextPropertyChanged(
      ShadowNodeBase *node,
      PropertyChangeType propertyChangeType = PropertyChangeType::Text);

  TextTransform GetTextTransformValue(ShadowNodeBase *node);

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;
};

} // namespace Microsoft::ReactNative
