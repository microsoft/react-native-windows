
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/components/text/ParagraphShadowNode.h>
#pragma warning(pop)

#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>

namespace Microsoft::ReactNative {

ParagraphComponentView::ParagraphComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ParagraphComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ParagraphComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  auto v = static_cast<const ParagraphComponentView &>(childComponentView);
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void ParagraphComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void ParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  if (oldViewProps.foregroundColor != newViewProps.foregroundColor) {
    if (newViewProps.foregroundColor)
      m_element.Foreground(::react::uwp::SolidColorBrushFrom(newViewProps.backgroundColor));
    else
      m_element.ClearValue(::xaml::Controls::TextBlock::ForegroundProperty());
  }

  m_props = std::static_pointer_cast<facebook::react::ParagraphProps const>(props);
}

void ParagraphComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}
void ParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  // Only handle single fragments right now
  assert(newState.getData().attributedString.getFragments().size() == 1);
  auto fragment = newState.getData().attributedString.getFragments()[0];

  m_element.Text(Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string));
}
void ParagraphComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_element, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_element, layoutMetrics.frame.origin.y);

  m_element.Width(layoutMetrics.frame.size.width);
  m_element.Height(layoutMetrics.frame.size.height);
}
void ParagraphComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void ParagraphComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ParagraphComponentView::props() noexcept {
  assert(false);
  return {};
}

const xaml::FrameworkElement ParagraphComponentView::Element() const noexcept {
  return m_element;
}

} // namespace Microsoft::ReactNative
