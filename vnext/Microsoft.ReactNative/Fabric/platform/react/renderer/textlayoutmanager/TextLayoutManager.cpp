// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <Fabric/DWriteHelpers.h>
#include <Utils/TransformableText.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <react/renderer/telemetry/TransactionTelemetry.h>
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

  if (!isnan(outerFragment.textAttributes.lineHeight)) {
    winrt::check_hresult(spTextFormat->SetLineSpacing(
        DWRITE_LINE_SPACING_METHOD_UNIFORM,
        outerFragment.textAttributes.lineHeight,
        // Recommended ratio of baseline to lineSpacing is 80%
        // https://learn.microsoft.com/en-us/windows/win32/api/dwrite/nf-dwrite-idwritetextformat-getlinespacing
        // It is possible we need to load full font metrics to calculate a better baseline value.
        // For a particular font, you can determine what lineSpacing and baseline should be by examining a
        // DWRITE_FONT_METRICS method available from the GetMetrics method of IDWriteFont or IDWriteFontFace. For normal
        // behavior, you'd set lineSpacing to the sum of ascent, descent and lineGap (adjusted for the em size, of
        // course), and baseline to the ascent value.
        outerFragment.textAttributes.lineHeight * 0.8f));
  }

  DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
  if (outerFragment.textAttributes.alignment) {
    switch (*outerFragment.textAttributes.alignment) {
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

  auto str = GetTransformedText(attributedStringBox);

  winrt::check_hresult(Microsoft::ReactNative::DWriteFactory()->CreateTextLayout(
      str.c_str(), // The string to be laid out and formatted.
      static_cast<UINT32>(str.size()), // The length of the string.
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

    if (!isnan(attributes.letterSpacing)) {
      winrt::check_hresult(
          spTextLayout.as<IDWriteTextLayout1>()->SetCharacterSpacing(0, attributes.letterSpacing, 0, range));
    }

    position += length;
  }
}

TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints,
    std::shared_ptr<void> /* hostTextStorage */) const {
  TextMeasurement measurement{};
  auto &attributedString = attributedStringBox.getValue();
  measurement = m_measureCache.get(
      {attributedString, paragraphAttributes, layoutConstraints}, [&](TextMeasureCacheKey const &key) {
        auto telemetry = TransactionTelemetry::threadLocalTelemetry();
        if (telemetry) {
          telemetry->willMeasureText();
        }

        winrt::com_ptr<IDWriteTextLayout> spTextLayout;

        GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints, spTextLayout);

        if (spTextLayout) {
          auto maxHeight = std::numeric_limits<float>().max();
          if (paragraphAttributes.maximumNumberOfLines > 0) {
            std::vector<DWRITE_LINE_METRICS> lineMetrics;
            uint32_t actualLineCount;
            spTextLayout->GetLineMetrics(nullptr, 0, &actualLineCount);
            lineMetrics.resize(static_cast<size_t>(actualLineCount));
            winrt::check_hresult(spTextLayout->GetLineMetrics(lineMetrics.data(), actualLineCount, &actualLineCount));
            maxHeight = 0;
            const auto count =
                std::min(static_cast<uint32_t>(paragraphAttributes.maximumNumberOfLines), actualLineCount);
            for (uint32_t i = 0; i < count; ++i) {
              maxHeight += lineMetrics[i].height;
            }
          }

          DWRITE_TEXT_METRICS dtm{};
          winrt::check_hresult(spTextLayout->GetMetrics(&dtm));
          measurement.size = {dtm.width, std::min(dtm.height, maxHeight)};
        }

        if (telemetry) {
          telemetry->didMeasureText();
        }

        return measurement;
      });

  return measurement;
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

std::shared_ptr<void> TextLayoutManager::getHostTextStorage(
    AttributedString attributedString,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  return nullptr;
}

void *TextLayoutManager::getNativeTextLayoutManager() const {
  return (void *)this;
}

Microsoft::ReactNative::TextTransform ConvertTextTransform(std::optional<TextTransform> const &transform) {
  if (transform) {
    switch (transform.value()) {
      case TextTransform::Capitalize:
        return Microsoft::ReactNative::TextTransform::Capitalize;
      case TextTransform::Lowercase:
        return Microsoft::ReactNative::TextTransform::Lowercase;
      case TextTransform::Uppercase:
        return Microsoft::ReactNative::TextTransform::Uppercase;
      case TextTransform::None:
        return Microsoft::ReactNative::TextTransform::None;
      default:
        break;
    }
  }

  return Microsoft::ReactNative::TextTransform::Undefined;
}

winrt::hstring TextLayoutManager::GetTransformedText(AttributedStringBox const &attributedStringBox) {
  winrt::hstring result{};
  for (const auto &fragment : attributedStringBox.getValue().getFragments()) {
    result = result +
        Microsoft::ReactNative::TransformableText::TransformText(
                 winrt::hstring{Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string)},
                 ConvertTextTransform(fragment.textAttributes.textTransform));
  }
  return result;
}

} // namespace facebook::react
