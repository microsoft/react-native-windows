
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ActivityIndicatorComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <react/components/rnwcore/Props.h>

namespace Microsoft::ReactNative {

ActivityIndicatorComponentView::ActivityIndicatorComponentView() : m_element(xaml::Controls::ProgressRing()) {
  static auto const defaultProps = std::make_shared<facebook::react::ActivityIndicatorViewProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ActivityIndicatorComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ActivityIndicatorComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void ActivityIndicatorComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldActivityProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(m_props);
  const auto &newActivityProps = *std::static_pointer_cast<const facebook::react::ActivityIndicatorViewProps>(props);

  if (oldActivityProps.animating != newActivityProps.animating) {
    m_element.IsActive(newActivityProps.animating);
  }

  if (oldActivityProps.color != newActivityProps.color) {
    m_element.Foreground(newActivityProps.color.AsWindowsBrush());
  }

  m_props = std::static_pointer_cast<facebook::react::ActivityIndicatorViewProps const>(props);
}

void ActivityIndicatorComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}
void ActivityIndicatorComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void ActivityIndicatorComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}
void ActivityIndicatorComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ActivityIndicatorComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement ActivityIndicatorComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
