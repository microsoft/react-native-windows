
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ScrollViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <unicode.h>

namespace Microsoft::ReactNative {

ScrollViewComponentView::ScrollViewComponentView() {
  //static auto const defaultProps = std::make_shared<facebook::react::TextProps const>();
  //m_props = defaultProps;

  m_element.Content(m_contentPanel);
}

std::vector<facebook::react::ComponentDescriptorProvider>
ScrollViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ScrollViewComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_contentPanel.Children().InsertAt(index, static_cast<const BaseComponentView &>(childComponentView).Element());
}

void ScrollViewComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_contentPanel.Children().RemoveAt(index);
}

void ScrollViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  //const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::TextProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);

  /*

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
  */
  //m_props = std::static_pointer_cast<facebook::react::TextProps const>(props);
}

void ScrollViewComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}
void ScrollViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
}
void ScrollViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void ScrollViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {

  //m_element.FinalizeProperties();

}
void ScrollViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ScrollViewComponentView::props() noexcept {
  assert(false);
  return {};
}


const xaml::FrameworkElement ScrollViewComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
