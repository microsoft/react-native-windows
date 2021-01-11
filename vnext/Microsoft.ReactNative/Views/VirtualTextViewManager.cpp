// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
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
  auto view = static_cast<ShadowNodeBase &>(child).GetView();

  if (auto run = view.try_as<xaml::Documents::Run>()) {
    transformableText.originalText = run.Text().c_str();
    run.Text(transformableText.TransformText());
  } else if (auto span = view.try_as<xaml::Documents::Span>()) {
    auto &childVTSN = static_cast<VirtualTextShadowNode &>(child);
    auto transform = childVTSN.transformableText.textTransform;

    for (const auto &i : span.Inlines()) {
      if (auto run = i.try_as<xaml::Documents::Run>()) {
        if (transform == TransformableText::TextTransform::Undefined) {
          // project the parent transform onto the child
          childVTSN.transformableText.textTransform = transformableText.textTransform;
          run.Text(childVTSN.transformableText.TransformText());
        }
      }
    }

    m_highlightData.data.emplace_back(childVTSN.m_highlightData);
  }
  Super::AddView(child, index);
}

VirtualTextViewManager::VirtualTextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *VirtualTextViewManager::GetName() const {
  return L"RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t /*tag*/) {
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
    node->transformableText.textTransform = TransformableText::GetTextTransform(propertyValue);
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
