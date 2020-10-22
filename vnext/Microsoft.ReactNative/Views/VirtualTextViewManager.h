// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <ShadowNodeBase.h>
#include <UI.Xaml.Documents.h>
#include <Utils/TransformableText.h>
#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

struct VirtualTextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  TransformableText transformableText{};

  void AddView(ShadowNode &child, int64_t index) override;

  struct HighlightData {
    std::vector<HighlightData> data;
    size_t spanIdx = 0;
    std::optional<winrt::Windows::UI::Color> color;
  };

  HighlightData m_highlightData;
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

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace Microsoft::ReactNative
