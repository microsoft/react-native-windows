// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiPortalShadowNode.h"

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <react/debug/react_native_assert.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/core/conversions.h>

#include <utility>

namespace Microsoft::ReactNative {

extern const char AbiPortalComponentName[] = "AbiPortal";

facebook::react::Size AbiPortalShadowNode::measureContent(
    const facebook::react::LayoutContext &layoutContext,
    const facebook::react::LayoutConstraints &layoutConstraints) const {
  return {0, 0}; // The portal placeholder node shouldn't take up any space
}

void AbiPortalShadowNode::layout(facebook::react::LayoutContext layoutContext) {
  ensureUnsealed();
  auto layoutMetrics = getLayoutMetrics();

  auto portalOwningShadowNode = ShadowNode::Unshared{};

  if (getChildren().empty()) {
    return;
  }

  // A Portal should only have a single child
  react_native_assert(getChildren().size() == 1);

  const auto &childNode = getChildren()[0];

  auto clonedShadowNode = ShadowNode::Unshared{};

  portalOwningShadowNode = cloneTree(childNode->getFamily(), [&](const ShadowNode &oldShadowNode) {
    clonedShadowNode = oldShadowNode.clone({});
    return clonedShadowNode;
  });
  auto portalShadowNode = static_cast<AbiPortalShadowNode *>(portalOwningShadowNode.get());

  auto &layoutableShadowNode = dynamic_cast<LayoutableShadowNode &>(*clonedShadowNode);

  auto &state = getStateData();

  facebook::react::LayoutConstraints layoutConstraints;
  layoutConstraints.layoutDirection = layoutMetrics.layoutDirection;

  if (state.userdata) {
    // If the portal component set a state of type IPortalStateData,
    // extract constraint information from it, and use that for layout
    if (auto portalState = state.userdata.try_as<winrt::Microsoft::ReactNative::Composition::IPortalStateData>()) {
      auto stateConstraints = portalState.LayoutConstraints();

      layoutConstraints.minimumSize = {stateConstraints.MinimumSize.Width, stateConstraints.MinimumSize.Height};
      layoutConstraints.maximumSize = {stateConstraints.MaximumSize.Width, stateConstraints.MaximumSize.Height};
      if (stateConstraints.LayoutDirection == winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight) {
        layoutConstraints.layoutDirection = facebook::react::LayoutDirection::LeftToRight;
      } else if (stateConstraints.LayoutDirection == winrt::Microsoft::ReactNative::LayoutDirection::RightToLeft) {
        layoutConstraints.layoutDirection = facebook::react::LayoutDirection::RightToLeft;
      }
    }
  }

  // Laying out the `ShadowNode` and the subtree starting from it.
  layoutableShadowNode.layoutTree(layoutContext, layoutConstraints);

  auto childLayoutMetrics = layoutableShadowNode.getLayoutMetrics();
  childLayoutMetrics.frame.origin = {0, 0};
  layoutableShadowNode.setLayoutMetrics(childLayoutMetrics);

  // Update the list of children to reflect the changes that we made.
  this->children_ = static_cast<AbiPortalShadowNode *>(portalOwningShadowNode.get())->children_;
}

void AbiPortalShadowNode::Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept {
  m_builder = builder;
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder AbiPortalShadowNode::Builder() const noexcept {
  return m_builder;
}

void AbiPortalShadowNode::Proxy(winrt::Microsoft::ReactNative::ShadowNode proxy) noexcept {
  m_proxy = proxy;
}

winrt::Microsoft::ReactNative::ShadowNode AbiPortalShadowNode::Proxy() const noexcept {
  return m_proxy;
}

} // namespace Microsoft::ReactNative
