
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "AbiCompositionViewComponentView.h"

#include <Fabric/DWriteHelpers.h>
#include "Unicode.h"

namespace Microsoft::ReactNative {

AbiCompositionViewComponentView::AbiCompositionViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::Composition::ICompositionViewComponent userComponent)
    : Super(compContext, tag), m_userComponent(userComponent) {
  static auto const defaultProps = std::make_shared<AbiViewProps const>();
  m_props = defaultProps;
  userComponent.Initialize(compContext);
  m_visual = userComponent.CreateVisual();
  OuterVisual().InsertAt(m_visual, 0);
}

void AbiCompositionViewComponentView::mountChildComponentView(
    IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void AbiCompositionViewComponentView::unmountChildComponentView(
    IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void AbiCompositionViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const AbiViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const AbiViewProps>(props);

  updateBorderProps(oldViewProps, newViewProps);
  m_userComponent.UpdateProps(newViewProps.UserProps());

  m_props = std::static_pointer_cast<AbiViewProps const>(props);

}

void AbiCompositionViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

  winrt::Microsoft::ReactNative::Composition::LayoutMetrics lm;
  m_userComponent.UpdateLayoutMetrics(
      {{layoutMetrics.frame.origin.x,
        layoutMetrics.frame.origin.y,
        layoutMetrics.frame.size.width,
        layoutMetrics.frame.size.height},
       layoutMetrics.pointScaleFactor});

  m_layoutMetrics = layoutMetrics;
}

void AbiCompositionViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
}

void AbiCompositionViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  m_userComponent.FinalizeUpdates();

  if (m_needsBorderUpdate) {
    m_needsBorderUpdate = false;
    UpdateSpecialBorderLayers(m_layoutMetrics, *m_props);
  }
}

bool AbiCompositionViewComponentView::focusable() const noexcept {
  return m_props->focusable;
}

  std::vector<facebook::react::ComponentDescriptorProvider>
AbiCompositionViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}


void AbiCompositionViewComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared AbiCompositionViewComponentView::props() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual AbiCompositionViewComponentView::Visual() const noexcept {
  return m_visual;
}

facebook::react::Tag AbiCompositionViewComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

} // namespace Microsoft::ReactNative
