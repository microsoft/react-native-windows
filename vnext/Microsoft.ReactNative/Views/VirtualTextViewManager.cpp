// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

#include <UI.Xaml.Documents.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ShadowNodeTypeUtils.h>
#include <Utils/TransformableText.h>
#include <Utils/ValueUtils.h>
#include <Views/Text/TextVisitors.h>

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {

void VirtualTextShadowNode::AddView(ShadowNode &child, int64_t index) {
  auto &childNode = static_cast<ShadowNodeBase &>(child);
  ApplyTextTransformToChild(&child);
  auto propertyChangeType = PropertyChangeType::Text;
  if (IsVirtualTextShadowNode(&childNode)) {
    const auto &childTextNode = static_cast<VirtualTextShadowNode &>(childNode);
    propertyChangeType |=
        childTextNode.hasDescendantTextHighlighter ? PropertyChangeType::AddHighlight : PropertyChangeType::None;
    propertyChangeType |=
        childTextNode.hasDescendantPressable ? PropertyChangeType::AddPressable : PropertyChangeType::None;
  }
  Super::AddView(child, index);
  NotifyAncestorsTextPropertyChanged(this, propertyChangeType);
}

void VirtualTextShadowNode::RemoveChildAt(int64_t indexToRemove) {
  Super::RemoveChildAt(indexToRemove);
  NotifyAncestorsTextPropertyChanged(this, PropertyChangeType::Text);
}

void VirtualTextShadowNode::removeAllChildren() {
  Super::removeAllChildren();
  NotifyAncestorsTextPropertyChanged(this, PropertyChangeType::Text);
}

VirtualTextViewManager::VirtualTextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *VirtualTextViewManager::GetName() const {
  return L"RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  return winrt::Span();
}

void VirtualTextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    winrt::Microsoft::ReactNative::JSValueObject &props) {
  // This could be optimized further, but rather than paying a penalty to mark
  // the node dirty for each relevant property in UpdateProperty (which should
  // be reasonably cheap given it just does an O(1) lookup of the Yoga node
  // for the tag, for now this just marks the node dirty for any prop update.
  MarkDirty(nodeToUpdate->m_tag);
  Super::UpdateProperties(nodeToUpdate, props);
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
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->foregroundColor = OptionalColorFrom(propertyValue);
      const auto propertyChangeType =
          node->foregroundColor ? PropertyChangeType::AddHighlight : PropertyChangeType::RemoveHighlight;
      NotifyAncestorsTextPropertyChanged(node, propertyChangeType);
    }
  } else if (TryUpdateFontProperties<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateCharacterSpacing<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    node->textTransform = TransformableText::GetTextTransform(propertyValue);
    UpdateTextTransformForChildren(nodeToUpdate);
  } else if (propertyName == "backgroundColor") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->backgroundColor = OptionalColorFrom(propertyValue);
      const auto propertyChangeType =
          node->backgroundColor ? PropertyChangeType::AddHighlight : PropertyChangeType::RemoveHighlight;
      NotifyAncestorsTextPropertyChanged(node, propertyChangeType);
    }
  } else if (propertyName == "isPressable") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    node->isPressable = propertyValue.AsBoolean();
    if (node->isPressable) {
      NotifyAncestorsTextPropertyChanged(node, PropertyChangeType::AddPressable);
    }
  } else {
    const auto isRegisteringMouseEvent =
        (propertyName == "onMouseEnter" || propertyName == "onMouseLeave") && propertyValue.AsBoolean();
    if (isRegisteringMouseEvent) {
      auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
      NotifyAncestorsTextPropertyChanged(node, PropertyChangeType::AddPressable);
    }

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
