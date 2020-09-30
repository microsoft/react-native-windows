// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "ContentControlViewManager.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Media;
} // namespace winrt

namespace react::uwp {
namespace polyester {

void ContentControlShadowNode::DoExtraLayoutPrep(YGNodeRef yogaNode) {
  if (!m_paddingDirty)
    return;

  m_paddingDirty = false;

  auto contentControl = GetView().try_as<xaml::Controls::ContentControl>();
  auto padding = contentControl.Padding();

  YGNodeStyleSetPadding(yogaNode, YGEdgeLeft, static_cast<float>(padding.Left));
  YGNodeStyleSetPadding(yogaNode, YGEdgeTop, static_cast<float>(padding.Top));
  YGNodeStyleSetPadding(yogaNode, YGEdgeRight, static_cast<float>(padding.Right));
  YGNodeStyleSetPadding(yogaNode, YGEdgeBottom, static_cast<float>(padding.Bottom));
}

void ContentControlShadowNode::createView() {
  Super::createView();

  auto obj = GetView().try_as<xaml::DependencyObject>();
  if (obj != nullptr) {
    // Button styles may have padding Yoga is not aware of
    obj.RegisterPropertyChangedCallback(
        xaml::Controls::Control::PaddingProperty(),
        [this](xaml::DependencyObject const & /*sender*/, xaml::DependencyProperty const & /*dp*/) {
          m_paddingDirty = true;
        });
  }
}

ContentControlViewManager::ContentControlViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

facebook::react::ShadowNode *ContentControlViewManager::createShadow() const {
  return new ContentControlShadowNode();
}

void ContentControlViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  // ContentControl holds a single child, so should never insert after
  if (index != 0) {
    // ASSERT: Currently considering any index other than 0 as ignorable since
    // ContentControl can only have a single child.
    assert(false);
    return;
  }

  auto contentControl(parent.as<xaml::Controls::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(child.as<winrt::IInspectable>());
}

void ContentControlViewManager::RemoveAllChildren(const XamlView &parent) {
  auto contentControl(parent.as<xaml::Controls::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(nullptr);
}

void ContentControlViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  if (index != 0)
    return;

  auto contentControl(parent.as<xaml::Controls::ContentControl>());
  if (contentControl != nullptr)
    contentControl.Content(nullptr);
}

} // namespace polyester
} // namespace react::uwp
