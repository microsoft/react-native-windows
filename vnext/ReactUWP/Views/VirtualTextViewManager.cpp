// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "VirtualTextViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
} // namespace winrt

namespace react {
namespace uwp {

VirtualTextViewManager::VirtualTextViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *VirtualTextViewManager::GetName() const {
  return "RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t tag) {
  return winrt::Span();
}

void VirtualTextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto span = nodeToUpdate->GetView().as<winrt::Span>();
  if (span == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    // FUTURE: In the future cppwinrt will generate code where static methods on
    // base types can be called.  For now we specify the base type explicitly
    if (TryUpdateForeground<winrt::TextElement>(
            span, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateFontProperties<winrt::TextElement>(
                   span, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing<winrt::TextElement>(
                   span, propertyName, propertyValue)) {
      continue;
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void VirtualTextViewManager::AddView(
    XamlView parent,
    XamlView child,
    int64_t index) {
  auto span(parent.as<winrt::Span>());
  auto childInline(child.as<winrt::Inline>());
  span.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

void VirtualTextViewManager::RemoveAllChildren(XamlView parent) {
  auto span(parent.as<winrt::Span>());
  span.Inlines().Clear();
}

void VirtualTextViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  auto span(parent.as<winrt::Span>());
  return span.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

bool VirtualTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace uwp
} // namespace react
