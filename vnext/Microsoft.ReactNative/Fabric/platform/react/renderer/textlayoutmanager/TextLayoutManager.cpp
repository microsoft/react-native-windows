// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include "TextLayoutManager.h"

#include <unicode.h>

namespace facebook::react {

TextLayoutManager::~TextLayoutManager() {}

TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  winrt::com_ptr<IDWriteFactory> spDWriteFactory;

  if (!m_spDWriteFactory) {
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(m_spDWriteFactory.put()));
  }

  for (auto &fragment : attributedStringBox.getValue().getFragments()) {
    DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
    if (fragment.textAttributes.fontStyle == facebook::react::FontStyle::Italic)
      style = DWRITE_FONT_STYLE_ITALIC;
    else if (fragment.textAttributes.fontStyle == facebook::react::FontStyle::Oblique)
      style = DWRITE_FONT_STYLE_OBLIQUE;

    winrt::com_ptr<IDWriteTextFormat> spTextFormat;
    m_spDWriteFactory->CreateTextFormat(
        fragment.textAttributes.fontFamily.empty()
            ? L"Segoe UI"
            : Microsoft::Common::Unicode::Utf8ToUtf16(fragment.textAttributes.fontFamily).c_str(),
        nullptr, // Font collection (nullptr sets it to use the system font collection).
        static_cast<DWRITE_FONT_WEIGHT>(fragment.textAttributes.fontWeight.value_or(
            static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
        style,
        DWRITE_FONT_STRETCH_NORMAL,
        fragment.textAttributes.fontSize * fragment.textAttributes.fontSizeMultiplier,
        L"en-us",
        spTextFormat.put());

    auto str = Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string);

    winrt::com_ptr<IDWriteTextLayout> spTextLayout;
    // TODO - For now assuming fragment.textAttributes.fontSizeMultiplier is the same as the pointScaleFactor
    m_spDWriteFactory->CreateTextLayout(
        str.c_str(), // The string to be laid out and formatted.
        static_cast<UINT32>(str.length()), // The length of the string.
        spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
        layoutConstraints.maximumSize.width *
            fragment.textAttributes.fontSizeMultiplier, // The width of the layout box.
        layoutConstraints.maximumSize.height *
            fragment.textAttributes.fontSizeMultiplier, // The height of the layout box.
        spTextLayout.put() // The IDWriteTextLayout interface pointer.
    );

    TextMeasurement tm;
    DWRITE_TEXT_METRICS dtm;
    spTextLayout->GetMetrics(&dtm);
    tm.size = {
        dtm.width / fragment.textAttributes.fontSizeMultiplier,
        dtm.height / fragment.textAttributes.fontSizeMultiplier};
    return tm;
  }

  return {};
};

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
