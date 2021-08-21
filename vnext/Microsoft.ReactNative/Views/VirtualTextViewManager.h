// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <UI.Xaml.Documents.h>
#include <Utils/TextTransform.h>
#include <Views/FrameworkElementViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/TextViewManager.h>

namespace Microsoft::ReactNative {

struct VirtualTextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  TextTransform textTransform{TextTransform::Undefined};

  void AddView(ShadowNode &child, int64_t index) override;
  void RemoveChildAt(int64_t indexToRemove) override;
  void removeAllChildren() override;

  void NotifyAncestorsTextPropertyChanged(PropertyChangeType propertyChangeType);

  static void ApplyTextTransform(ShadowNodeBase &node, TextTransform transform, bool forceUpdate, bool isRoot);

  std::optional<winrt::Windows::UI::Color> m_backgroundColor;
  std::optional<winrt::Windows::UI::Color> m_foregroundColor;
  bool m_hasDescendantBackgroundColor{false};
};

class VirtualTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  VirtualTextViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;
  ShadowNode *createShadow() const override {
    return new VirtualTextShadowNode();
  }

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  bool RequiresYogaNode() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  XamlView CreateViewCore(int64_t tag, const winrt::Microsoft::ReactNative::JSValueObject &) override;
};

} // namespace Microsoft::ReactNative
