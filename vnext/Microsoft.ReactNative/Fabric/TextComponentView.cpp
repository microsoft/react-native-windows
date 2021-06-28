
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

namespace Microsoft::ReactNative {

TextComponentView::TextComponentView() {
  // static auto const defaultProps = std::make_shared<facebook::react::TextProps const>();
  // m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
TextComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void TextComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  auto v = static_cast<const TextComponentView &>(childComponentView);
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void TextComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void TextComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  /*
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::TextProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::TextProps>(props);


  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    auto color = *newViewProps.backgroundColor;

    if (newViewProps.backgroundColor) {
      m_element.ViewBackground(SolidColorBrushFrom(newViewProps.backgroundColor));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::ViewBackgroundProperty());
    }
  }

  if (oldViewProps.borderColors != newViewProps.borderColors) {
    if (newViewProps.borderColors.all) {
      m_element.BorderBrush(SolidColorBrushFrom(*newViewProps.borderColors.all));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::BorderBrushProperty());
    }
  }

  if (oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsBorderUpdate = true;
  }
  */
  // m_props = std::static_pointer_cast<facebook::react::TextProps const>(props);
}

void TextComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}
void TextComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}
void TextComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void TextComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void TextComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps TextComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement TextComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
