// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"

namespace Microsoft::ReactNative {

ActivityIndicatorComponentView::ActivityIndicatorComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  m_props = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
}

std::vector<facebook::react::ComponentDescriptorProvider>
ActivityIndicatorComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ActivityIndicatorComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  ensureVisual();

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

void ActivityIndicatorComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void ActivityIndicatorComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void ActivityIndicatorComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  ensureDrawingSurface();
}

void ActivityIndicatorComponentView::Draw() noexcept {

}

void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared ActivityIndicatorComponentView::props() noexcept {
  return m_props;
}

void ActivityIndicatorComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
  }
}

void ActivityIndicatorComponentView::ensureDrawingSurface() noexcept {

}

facebook::react::Tag ActivityIndicatorComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept {

  return -1;
}

winrt::Microsoft::ReactNative::Composition::IVisual ActivityIndicatorComponentView::Visual() const noexcept {
  return m_visual;
}

int64_t ActivityIndicatorComponentView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  return 0;
}


} // namespace Microsoft::ReactNative
