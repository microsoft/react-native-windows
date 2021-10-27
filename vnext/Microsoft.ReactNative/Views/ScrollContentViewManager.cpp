// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollContentViewManager.h"

#include <UI.Xaml.Controls.h>
#include <Views/ViewViewManager.h>
#include "Impl/SnapPointManagingContentControl.h"
#include "ViewPanel.h"

namespace Microsoft::ReactNative {

ScrollContentViewManager::ScrollContentViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ScrollContentViewManager::GetName() const {
  return L"RCTScrollContentView";
}

XamlView ScrollContentViewManager::CreateViewCore(
    int64_t /*tag*/,
    const winrt::Microsoft::ReactNative::JSValueObject & /*props*/) {
  auto panel = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewPanel>();
  panel.VerticalAlignment(xaml::VerticalAlignment::Stretch);
  panel.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
  return panel.as<XamlView>();
}

void ScrollContentViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  // All top-level children of inverted ScrollView content will be anchor candidates, unless scrolled to the top.
  auto childElement = child.as<xaml::UIElement>();
  auto viewParent = parent.as<xaml::FrameworkElement>().Parent();
  if (viewParent) {
    const auto scrollViewContentControl = viewParent.as<SnapPointManagingContentControl>();
    if (scrollViewContentControl->IsInverted() && scrollViewContentControl->IsContentAnchoringEnabled()) {
      if (winrt::unbox_value<bool>(child.GetValue(ViewViewManager::CanBeScrollAnchorProperty()))) {
        childElement.CanBeScrollAnchor(true);
      }
    }
  }

  parent.as<winrt::Microsoft::ReactNative::ViewPanel>().InsertAt(static_cast<uint32_t>(index), childElement);
}

void ScrollContentViewManager::RemoveAllChildren(const XamlView &parent) {
  parent.as<winrt::Microsoft::ReactNative::implementation::ViewPanel>()->Clear();
}

void ScrollContentViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  parent.as<winrt::Microsoft::ReactNative::implementation::ViewPanel>()->RemoveAt(static_cast<uint32_t>(index));
}

} // namespace Microsoft::ReactNative
