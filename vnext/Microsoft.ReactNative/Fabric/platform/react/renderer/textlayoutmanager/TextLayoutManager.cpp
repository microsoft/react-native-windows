// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <dwrite.h>
#include "TextLayoutManager.h"

#include <CppWinrtIncludes.h>
#include <UI.Xaml.Controls.h>
#include <unicode.h>

namespace facebook::react {

TextLayoutManager::~TextLayoutManager() {}

// Ideally we'd be able to measure Text either without creating a XAML element,
// or we'd be able to create a XAML element on a background thread.
//
// For now we've forced the background executor to be the UI thread, so that we can use
// TextBlocks within the measure call.
//
// There will be inconsistencies with layout if any property that affects layout is set differently here vs in the
// actual view component. -- Any properties that rely on the context from the UI tree should be set directly on the
// TextBlock here and in the view component.
TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  for (auto &fragment : attributedStringBox.getValue().getFragments()) {
    if (!m_textBlock) {
      m_textBlock = xaml::Controls::TextBlock();
    }

    auto textblock = xaml::Controls::TextBlock();
    m_textBlock.Text(winrt::to_hstring(fragment.string));
    m_textBlock.FontSize(fragment.textAttributes.fontSize);
    m_textBlock.FontWeight(
        winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(fragment.textAttributes.fontWeight.value_or(
            static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR)))});

    switch (fragment.textAttributes.fontStyle.value_or(facebook::react::FontStyle::Normal)) {
      case facebook::react::FontStyle::Italic:
        m_textBlock.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
        break;
      case facebook::react::FontStyle::Normal:
        m_textBlock.FontStyle(winrt::Windows::UI::Text::FontStyle::Normal);
        break;
      case facebook::react::FontStyle::Oblique:
        m_textBlock.FontStyle(winrt::Windows::UI::Text::FontStyle::Oblique);
        break;
      default:
        assert(false);
    }

    if (fragment.textAttributes.fontFamily.empty())
      m_textBlock.FontFamily(xaml::Media::FontFamily(L"Segoe UI"));
    else
      m_textBlock.FontFamily(
          xaml::Media::FontFamily(Microsoft::Common::Unicode::Utf8ToUtf16(fragment.textAttributes.fontFamily)));

    winrt::Windows::Foundation::Size availiableSize(
        layoutConstraints.maximumSize.width, layoutConstraints.maximumSize.height);
    m_textBlock.Measure(availiableSize);
    TextMeasurement tm;
    auto size = m_textBlock.DesiredSize();

    tm.size = {static_cast<float>(size.Width), static_cast<float>(size.Height)};
    return tm;
  }

  return {};
};

LinesMeasurements TextLayoutManager::measureLines(
    AttributedString attributedString,
    ParagraphAttributes paragraphAttributes,
    Size size) const {
  assert(false);

  std::vector<LineMeasurement> paragraphLines{};
  return paragraphLines;
}

void *TextLayoutManager::getNativeTextLayoutManager() const {
  return (void *)this;
}

} // namespace facebook::react
