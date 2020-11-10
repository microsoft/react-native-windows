
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

namespace Microsoft::ReactNative {

void BaseComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
}

ViewComponentView::ViewComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::ViewProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ViewComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_element.Children().InsertAt(index, static_cast<const BaseComponentView &>(childComponentView).Element());
}

void ViewComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_element.Children().RemoveAt(index);
}

void ViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    auto color = *newViewProps.backgroundColor;

    if (newViewProps.backgroundColor) {
      m_element.ViewBackground(::react::uwp::SolidColorBrushFrom(newViewProps.backgroundColor));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::ViewBackgroundProperty());
    }
  }

  if (oldViewProps.borderColors != newViewProps.borderColors) {
    if (newViewProps.borderColors.all) {
      m_element.BorderBrush(::react::uwp::SolidColorBrushFrom(*newViewProps.borderColors.all));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::BorderBrushProperty());
    }
  }

  if (oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsBorderUpdate = true;
  }

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

void ViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}
void ViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void ViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  if (m_needsBorderUpdate) {
    m_needsBorderUpdate = false;
    auto const borderMetrics = m_props->resolveBorderMetrics(m_layoutMetrics);
    m_element.BorderThickness(xaml::ThicknessHelper::FromLengths(
        borderMetrics.borderWidths.left,
        borderMetrics.borderWidths.top,
        borderMetrics.borderWidths.right,
        borderMetrics.borderWidths.bottom));
  }

  m_element.FinalizeProperties();
}
void ViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ViewComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement ViewComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
