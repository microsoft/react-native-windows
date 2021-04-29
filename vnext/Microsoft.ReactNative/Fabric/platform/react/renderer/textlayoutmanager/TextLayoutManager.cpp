// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include "TextLayoutManager.h"

#include <dwrite.h>

#include <unicode.h>

namespace facebook {
namespace react {

TextLayoutManager::~TextLayoutManager() {}

// TODO: This is a placeholder implementation to get basic text rendering.  This should not be considered a real
// implementation. It currently has various performance issues (like creating a new factory on every run), and only
// handles single strings, and makes all kinds of assumptions around things like locale, font family etc. This whole
// implementation will need to be replaced to get text rendering/layout to work properly.
TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  winrt::com_ptr<IDWriteFactory> spDWriteFactory;
  DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(spDWriteFactory.put()));

  for (auto &fragment : attributedStringBox.getValue().getFragments()) {
    DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
    if (fragment.textAttributes.fontStyle == facebook::react::FontStyle::Italic)
      style = DWRITE_FONT_STYLE_ITALIC;
    else if (fragment.textAttributes.fontStyle == facebook::react::FontStyle::Oblique)
      style = DWRITE_FONT_STYLE_OBLIQUE;

    winrt::com_ptr<IDWriteTextFormat> spTextFormat;
    spDWriteFactory->CreateTextFormat(
        fragment.textAttributes.fontFamily.empty()
            ? L"Segoe UI"
            : Microsoft::Common::Unicode::Utf8ToUtf16(fragment.textAttributes.fontFamily).c_str(),
        NULL, // Font collection (NULL sets it to use the system font collection).
        static_cast<DWRITE_FONT_WEIGHT>(fragment.textAttributes.fontWeight.value_or(
            static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
        style,
        DWRITE_FONT_STRETCH_NORMAL,
        fragment.textAttributes.fontSize,
        L"en-us",
        spTextFormat.put());

    auto str = Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string);

    winrt::com_ptr<IDWriteTextLayout> spTextLayout;
    spDWriteFactory->CreateTextLayout(
        str.c_str(), // The string to be laid out and formatted.
        static_cast<UINT32>(str.length()), // The length of the string.
        spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
        layoutConstraints.maximumSize.width, // The width of the layout box.
        layoutConstraints.maximumSize.height, // The height of the layout box.
        spTextLayout.put() // The IDWriteTextLayout interface pointer.
    );

    TextMeasurement tm;

    DWRITE_TEXT_METRICS dtm;
    spTextLayout->GetMetrics(&dtm);
    tm.size = {dtm.width, dtm.height};
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

} // namespace react
} // namespace facebook
