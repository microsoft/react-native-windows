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

  bool m_isHighlighted = false;
  folly::dynamic m_ColorValue = nullptr;
  size_t m_length = 0;

  typedef struct HighlightData {
    std::vector<std::unique_ptr<HighlightData>> data;
    size_t spanIdx = 0;
    bool isHighlighted = false;
    folly::dynamic color;
  } HighlightData;

  std::unique_ptr<HighlightData> m_highlightData = std::make_unique<HighlightData>();
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
