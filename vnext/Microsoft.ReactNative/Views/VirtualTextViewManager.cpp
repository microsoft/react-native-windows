// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

#include <Views/RawTextViewManager.h>

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.h>
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
  auto propertyChangeType = PropertyChangeType::Text;

  // TODO: what if current is undefined, and already attached to root
  // We must search for the first inherited text transform...
  ApplyTextTransformsToChild(&child, textTransform);

  if (IsVirtualTextShadowNode(&childNode)) {
    const auto &childTextNode = static_cast<VirtualTextShadowNode &>(childNode);
    m_hasDescendantBackgroundColor |= childTextNode.m_hasDescendantBackgroundColor;
    propertyChangeType |=
        childTextNode.m_backgroundColor ? PropertyChangeType::AddBackgroundColor : PropertyChangeType::None;
  }
  Super::AddView(child, index);
  NotifyAncestorsTextPropertyChanged(propertyChangeType);
}

void VirtualTextShadowNode::RemoveChildAt(int64_t indexToRemove) {
  Super::RemoveChildAt(indexToRemove);
  NotifyAncestorsTextPropertyChanged(PropertyChangeType::Text);
}

void VirtualTextShadowNode::removeAllChildren() {
  Super::removeAllChildren();
  NotifyAncestorsTextPropertyChanged(PropertyChangeType::Text);
}

void VirtualTextShadowNode::NotifyAncestorsTextPropertyChanged(PropertyChangeType propertyChangeType) {
  if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
    auto host = uiManager->getHost();
    ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(m_parent));
    while (parent) {
      auto viewManager = parent->GetViewManager();
      const auto nodeType = viewManager->GetName();
      if (IsTextShadowNode(parent)) {
        (static_cast<TextViewManager *>(viewManager))->OnDescendantTextPropertyChanged(parent, propertyChangeType);
        break;
      } else if (IsVirtualTextShadowNode(parent)) {
        auto textParent = static_cast<VirtualTextShadowNode *>(parent);
        textParent->m_hasDescendantBackgroundColor |= m_hasDescendantBackgroundColor;
      }
      parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parent->GetParent()));
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
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->m_foregroundColor = OptionalColorFrom(propertyValue);
      node->m_hasDescendantBackgroundColor |= node->m_foregroundColor.has_value();
      const auto propertyChangeType =
          node->m_foregroundColor ? PropertyChangeType::AddBackgroundColor : PropertyChangeType::None;
      node->NotifyAncestorsTextPropertyChanged(propertyChangeType);
    }
  } else if (TryUpdateFontProperties<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateCharacterSpacing<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    node->textTransform = TransformableText::GetTextTransform(propertyValue);
    UpdateTextTransformsForAllChildren(nodeToUpdate);
  } else if (propertyName == "backgroundColor") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->m_backgroundColor = OptionalColorFrom(propertyValue);
      node->m_hasDescendantBackgroundColor |= node->m_backgroundColor.has_value();
      const auto propertyChangeType =
          node->m_backgroundColor ? PropertyChangeType::AddBackgroundColor : PropertyChangeType::None;
      node->NotifyAncestorsTextPropertyChanged(propertyChangeType);
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
