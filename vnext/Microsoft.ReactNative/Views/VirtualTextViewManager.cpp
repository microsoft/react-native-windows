// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

#include <Views/RawTextViewManager.h>

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {

void VirtualTextShadowNode::AddView(ShadowNode &child, int64_t index) {
  auto &childNode = static_cast<ShadowNodeBase &>(child);
  ApplyTextTransformToNode(childNode, textTransform, /* hasChanged = */false);
  if (auto span = childNode.GetView().try_as<xaml::Documents::Span>()) {
    auto &childVTSN = static_cast<VirtualTextShadowNode &>(child);
    m_highlightData.data.emplace_back(childVTSN.m_highlightData);
  }
  Super::AddView(child, index);
}

void VirtualTextShadowNode::ApplyTextTransform(TransformableText::TextTransform transform, bool hasChanged, bool inherited) {
    if (!inherited || textTransform == TransformableText::TextTransform::Undefined) {
    if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
      for (auto childTag : m_children) {
        const auto childNode = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(childTag));
        ApplyTextTransformToNode(*childNode, transform, hasChanged);
      }
    }
  }
}

void VirtualTextShadowNode::ApplyTextTransformToNode(
    ShadowNodeBase &node,
    TransformableText::TextTransform transform,
    bool hasChanged) {
  if (hasChanged ||
      (transform != TransformableText::TextTransform::Undefined &&
       transform != TransformableText::TextTransform::None)) {
    const auto view = node.GetView();
    const auto span = view.try_as<winrt::Span>();
    const auto run = view.try_as<winrt::Run>();
    if (span) {
      auto &virtualTextChildNode = static_cast<VirtualTextShadowNode &>(node);
      virtualTextChildNode.ApplyTextTransform(transform, hasChanged, /* inherited = */true);
    } else if (run) {
      auto &rawTextChildNode = static_cast<RawTextShadowNode &>(node);
      auto originalText = rawTextChildNode.originalText;
      if (originalText.size() != run.Text().size()) {
        // Lazily setting original text to avoid keeping two copies of all raw text strings
        originalText = run.Text();
        rawTextChildNode.originalText = originalText;
      }

      run.Text(TransformableText::TransformText(originalText, transform));

      if (transform == TransformableText::TextTransform::Undefined ||
          transform == TransformableText::TextTransform::None) {
        // If the transformed text is the same as the original, we no longer need a second copy
        rawTextChildNode.originalText = winrt::hstring{};
      }
    }
  }
}

VirtualTextViewManager::VirtualTextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *VirtualTextViewManager::GetName() const {
  return L"RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  return winrt::Span();
}

bool VirtualTextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto span = nodeToUpdate->GetView().as<winrt::Span>();
  if (span == nullptr)
    return true;

  // FUTURE: In the future cppwinrt will generate code where static methods on
  // base types can be called.  For now we specify the base type explicitly
  if (TryUpdateForeground<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateFontProperties<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateCharacterSpacing<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    node->textTransform = TransformableText::GetTextTransform(propertyValue);
    node->ApplyTextTransform(node->textTransform, /* hasChanged = */true, /* inherited = */false);
  } else if (propertyName == "backgroundColor") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      static_cast<VirtualTextShadowNode *>(nodeToUpdate)->m_highlightData.color = react::uwp::ColorFrom(propertyValue);
    }
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void VirtualTextViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto span(parent.as<winrt::Span>());
  auto childInline(child.as<winrt::Inline>());
  span.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

void VirtualTextViewManager::RemoveAllChildren(const XamlView &parent) {
  auto span(parent.as<winrt::Span>());
  span.Inlines().Clear();
}

void VirtualTextViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  auto span(parent.as<winrt::Span>());
  return span.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

bool VirtualTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace Microsoft::ReactNative
