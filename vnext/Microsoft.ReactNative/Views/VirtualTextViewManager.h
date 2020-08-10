// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <INativeUIManager.h>
#include <ShadowNodeBase.h>
#include <UI.Xaml.Documents.h>
#include <Utils/TransformableText.h>
#include <Views/FrameworkElementViewManager.h>

namespace react::uwp {

struct VirtualTextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  TransformableText m_transformableText{};
  void AddView(ShadowNode &child, int64_t index) override {
    auto &childSNB = static_cast<ShadowNodeBase &>(child);
    if (auto run = childSNB.GetView().try_as<xaml::Documents::Run>()) {
      m_transformableText.m_originalText = run.Text().c_str();
      run.Text(m_transformableText.TransformText());
    } else if (auto span = childSNB.GetView().try_as<xaml::Documents::Span>()) {
      auto &childVTSN = static_cast<VirtualTextShadowNode &>(child);
      auto transform = childVTSN.m_transformableText.m_textTransform;

      for (const auto &i : span.Inlines()) {
        if (auto run = i.try_as<xaml::Documents::Run>()) {
          if (transform == TransformableText::TextTransform::Undefined) {
            // project the parent transform onto the child
            childVTSN.m_transformableText.m_textTransform = m_transformableText.m_textTransform;
            run.Text(childVTSN.m_transformableText.TransformText());
          }
        }
      }
    }
    Super::AddView(child, index);
  }
};

class VirtualTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  VirtualTextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  facebook::react::ShadowNode *createShadow() const override {
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
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace react::uwp
