
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <dwrite.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
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

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    if (newViewProps.textAttributes.foregroundColor)
      m_element.Foreground(newViewProps.textAttributes.foregroundColor.AsWindowsBrush());
    else
      m_element.ClearValue(::xaml::Controls::TextBlock::ForegroundProperty());
  }

  if (oldViewProps.textAttributes.fontSize != newViewProps.textAttributes.fontSize) {
    if (std::isnan(newViewProps.textAttributes.fontSize))
      m_element.ClearValue(::xaml::Controls::TextBlock::FontSizeProperty());
    else
      m_element.FontSize(newViewProps.textAttributes.fontSize);
  }

  if (oldViewProps.textAttributes.fontWeight != newViewProps.textAttributes.fontWeight) {
    m_element.FontWeight(
        winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(newViewProps.textAttributes.fontWeight.value_or(
            static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR)))});
  }

  if (oldViewProps.textAttributes.fontStyle != newViewProps.textAttributes.fontStyle) {
    switch (newViewProps.textAttributes.fontStyle.value_or(facebook::react::FontStyle::Normal)) {
      case facebook::react::FontStyle::Italic:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
        break;
      case facebook::react::FontStyle::Normal:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Normal);
        break;
      case facebook::react::FontStyle::Oblique:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Oblique);
        break;
      default:
        assert(false);
    }
  }

  if (oldViewProps.textAttributes.fontFamily != newViewProps.textAttributes.fontFamily) {
    if (newViewProps.textAttributes.fontFamily.empty())
      m_element.FontFamily(xaml::Media::FontFamily(L"Segoe UI"));
    else
      m_element.FontFamily(
          xaml::Media::FontFamily(Microsoft::Common::Unicode::Utf8ToUtf16(newViewProps.textAttributes.fontFamily)));
  }

  m_props = std::static_pointer_cast<facebook::react::ParagraphProps const>(props);
}

void ParagraphComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}
void ParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  // Only handle single/empty fragments right now -- ignore the other fragments
  m_element.Text(
      newState.getData().attributedString.getFragments().size()
          ? Microsoft::Common::Unicode::Utf8ToUtf16(newState.getData().attributedString.getFragments()[0].string)
          : L"");
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
