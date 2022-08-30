// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <Fabric/DWriteHelpers.h>
#include <dwrite.h>
#include "TextLayoutManager.h"

#include <unicode.h>

namespace facebook::react {

void TextLayoutManager::GetTextLayout(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints,
    const std::optional<TextAlignment> &textAlignment,
    winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept {
  if (attributedStringBox.getValue().isEmpty())
    return;

  auto fragments = attributedStringBox.getValue().getFragments();
  auto outerFragment = fragments[0];

  DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
  if (outerFragment.textAttributes.fontStyle == facebook::react::FontStyle::Italic)
    style = DWRITE_FONT_STYLE_ITALIC;
  else if (outerFragment.textAttributes.fontStyle == facebook::react::FontStyle::Oblique)
    style = DWRITE_FONT_STYLE_OBLIQUE;

  winrt::com_ptr<IDWriteTextFormat> spTextFormat;
  winrt::check_hresult(Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
      outerFragment.textAttributes.fontFamily.empty()
          ? L"Segoe UI"
          : Microsoft::Common::Unicode::Utf8ToUtf16(outerFragment.textAttributes.fontFamily).c_str(),
      nullptr, // Font collection (nullptr sets it to use the system font collection).
      static_cast<DWRITE_FONT_WEIGHT>(outerFragment.textAttributes.fontWeight.value_or(
          static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
      style,
      DWRITE_FONT_STRETCH_NORMAL,
      outerFragment.textAttributes.fontSize,
      L"",
      spTextFormat.put()));

  DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
  if (textAlignment) {
    switch (*textAlignment) {
      case facebook::react::TextAlignment::Center:
        alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
        break;
      case facebook::react::TextAlignment::Justified:
        alignment = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
        break;
      case facebook::react::TextAlignment::Left:
        alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
      case facebook::react::TextAlignment::Right:
        alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
        break;
      // TODO use LTR values
      case facebook::react::TextAlignment::Natural:
        alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        break;
      default:
        assert(false);
    }
  }
  winrt::check_hresult(spTextFormat->SetTextAlignment(alignment));

  auto str = Microsoft::Common::Unicode::Utf8ToUtf16(attributedStringBox.getValue().getString());

  winrt::check_hresult(Microsoft::ReactNative::DWriteFactory()->CreateTextLayout(
      str.c_str(), // The string to be laid out and formatted.
      static_cast<UINT32>(str.length()), // The length of the string.
      spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
      layoutConstraints.maximumSize.width, // The width of the layout box.
      layoutConstraints.maximumSize.height, // The height of the layout box.
      spTextLayout.put() // The IDWriteTextLayout interface pointer.
      ));

  unsigned int position = 0;
  unsigned int length = 0;
  for (const auto &fragment : fragments) {
    length = static_cast<UINT32>(fragment.string.length());
    DWRITE_TEXT_RANGE range = {position, length};
    TextAttributes attributes = fragment.textAttributes;
    DWRITE_FONT_STYLE fragmentStyle = DWRITE_FONT_STYLE_NORMAL;
    if (attributes.fontStyle == facebook::react::FontStyle::Italic)
      fragmentStyle = DWRITE_FONT_STYLE_ITALIC;
    else if (attributes.fontStyle == facebook::react::FontStyle::Oblique)
      fragmentStyle = DWRITE_FONT_STYLE_OBLIQUE;

    winrt::check_hresult(spTextLayout->SetFontFamilyName(
        attributes.fontFamily.empty() ? L"Segoe UI"
                                      : Microsoft::Common::Unicode::Utf8ToUtf16(attributes.fontFamily).c_str(),
        range));
    winrt::check_hresult(spTextLayout->SetFontWeight(
        static_cast<DWRITE_FONT_WEIGHT>(
            attributes.fontWeight.value_or(static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
        range));
    winrt::check_hresult(spTextLayout->SetFontStyle(fragmentStyle, range));
    winrt::check_hresult(spTextLayout->SetFontSize(attributes.fontSize, range));

    position += length;
  }
}

TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  winrt::com_ptr<IDWriteTextLayout> spTextLayout;

  GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints, TextAlignment::Left, spTextLayout);

  TextMeasurement tm{};
  if (spTextLayout) {
    DWRITE_TEXT_METRICS dtm{};
    winrt::check_hresult(spTextLayout->GetMetrics(&dtm));
    tm.size = {dtm.width, dtm.height};
  }
  return tm;
}

/**
 * Measures an AttributedString on the platform, as identified by some
 * opaque cache ID.
 */
TextMeasurement TextLayoutManager::measureCachedSpannableById(
    int64_t cacheId,
    ParagraphAttributes const &paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  assert(false);
  return {};
}

LinesMeasurements TextLayoutManager::measureLines(
    AttributedString attributedString,
    ParagraphAttributes paragraphAttributes,
    Size size) const {
  assert(false);
  return {};
}

void *TextLayoutManager::getNativeTextLayoutManager() const {
  return (void *)this;
}

} // namespace facebook::react
