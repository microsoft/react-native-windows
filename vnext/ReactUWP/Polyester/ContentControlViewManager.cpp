// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "ContentControlViewManager.h"

#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {
namespace polyester {

void ContentControlShadowNode::DoExtraLayoutPrep(YGNodeRef yogaNode) {
  if (!m_paddingDirty)
    return;

  m_paddingDirty = false;

  auto contentControl = GetView().try_as<winrt::ContentControl>();
  auto padding = contentControl.Padding();

  YGNodeStyleSetPadding(yogaNode, YGEdgeLeft, static_cast<float>(padding.Left));
  YGNodeStyleSetPadding(yogaNode, YGEdgeTop, static_cast<float>(padding.Top));
  YGNodeStyleSetPadding(
      yogaNode, YGEdgeRight, static_cast<float>(padding.Right));
  YGNodeStyleSetPadding(
      yogaNode, YGEdgeBottom, static_cast<float>(padding.Bottom));
}

void ContentControlShadowNode::createView() {
  Super::createView();

  auto obj = GetView().try_as<winrt::DependencyObject>();
  if (obj != nullptr) {
    // Button styles may have padding Yoga is not aware of
    obj.RegisterPropertyChangedCallback(
        winrt::Windows::UI::Xaml::Controls::Control::PaddingProperty(),
        [this](
            winrt::Windows::UI::Xaml::DependencyObject const &sender,
            winrt::Windows::UI::Xaml::DependencyProperty const &dp) {
          m_paddingDirty = true;
        });
  }
}

ContentControlViewManager::ContentControlViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

facebook::react::ShadowNode *ContentControlViewManager::createShadow() const {
  return new ContentControlShadowNode();
}

void ContentControlViewManager::AddView(
    XamlView parent,
    XamlView child,
    int64_t index) {
  // ContentControl holds a single child, so should never insert after
  if (index != 0) {
    // ASSERT: Currently considering any index other than 0 as ignorable since
    // ContentControl can only have a single child.
    assert(false);
    return;
  }

  auto contentControl(parent.as<winrt::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(child.as<winrt::IInspectable>());
}

void ContentControlViewManager::RemoveAllChildren(XamlView parent) {
  auto contentControl(parent.as<winrt::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(nullptr);
}

void ContentControlViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  if (index != 0)
    return;

  auto contentControl(parent.as<winrt::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(nullptr);
}

} // namespace polyester
} // namespace uwp
} // namespace react
