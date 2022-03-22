// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <Fabric/CompHelpers.h>
#include <dwrite.h>
#include "TextLayoutManager.h"

#include <unicode.h>

namespace facebook::react {

void TextLayoutManager::GetTextLayout(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints,
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
  Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
      outerFragment.textAttributes.fontFamily.empty()
          ? L"Segoe UI"
          : Microsoft::Common::Unicode::Utf8ToUtf16(outerFragment.textAttributes.fontFamily).c_str(),
      NULL, // Font collection (NULL sets it to use the system font collection).
      static_cast<DWRITE_FONT_WEIGHT>(outerFragment.textAttributes.fontWeight.value_or(
          static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
      style,
      DWRITE_FONT_STRETCH_NORMAL,
      outerFragment.textAttributes.fontSize,
      L"en-us",
      spTextFormat.put());

  auto str = Microsoft::Common::Unicode::Utf8ToUtf16(attributedStringBox.getValue().getString());

  // TODO - For now assuming fragment.textAttributes.fontSizeMultiplier is the same as the pointScaleFactor
  Microsoft::ReactNative::DWriteFactory()->CreateTextLayout(
      str.c_str(), // The string to be laid out and formatted.
      static_cast<UINT32>(str.length()), // The length of the string.
      spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
      layoutConstraints.maximumSize.width *
          outerFragment.textAttributes.fontSizeMultiplier, // The width of the layout box.
      layoutConstraints.maximumSize.height *
          outerFragment.textAttributes.fontSizeMultiplier, // The height of the layout box.
      spTextLayout.put() // The IDWriteTextLayout interface pointer.
  );

  unsigned int position = 0;
  unsigned int length = 0;
  for (auto fragment : fragments) {
    length = static_cast<UINT32>(fragment.string.length());
    DWRITE_TEXT_RANGE range = {position, length};
    TextAttributes attributes = fragment.textAttributes;
    DWRITE_FONT_STYLE fragmentStyle = DWRITE_FONT_STYLE_NORMAL;
    if (attributes.fontStyle == facebook::react::FontStyle::Italic)
      fragmentStyle = DWRITE_FONT_STYLE_ITALIC;
    else if (attributes.fontStyle == facebook::react::FontStyle::Oblique)
      fragmentStyle = DWRITE_FONT_STYLE_OBLIQUE;

    spTextLayout->SetFontFamilyName(
        attributes.fontFamily.empty() ? L"Segoe UI"
                                      : Microsoft::Common::Unicode::Utf8ToUtf16(attributes.fontFamily).c_str(),
        range);
    spTextLayout->SetFontWeight(
        static_cast<DWRITE_FONT_WEIGHT>(
            attributes.fontWeight.value_or(static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
        range);
    spTextLayout->SetFontStyle(fragmentStyle, range);
    spTextLayout->SetFontSize(attributes.fontSize, range);

    position += length;
  }
}

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
  winrt::com_ptr<IDWriteTextLayout> spTextLayout;
  GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints, spTextLayout);

  TextMeasurement tm;
  if (spTextLayout) {
    DWRITE_TEXT_METRICS dtm;
    spTextLayout->GetMetrics(&dtm);
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
  TextMeasurement tm;
  return tm;
}

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
