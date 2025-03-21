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

// Creates an empty InlineObject since RN handles actually rendering the Inline object, this just reserves space for it.
class AttachmentInlineObject : public winrt::implements<AttachmentInlineObject, IDWriteInlineObject> {
 public:
  AttachmentInlineObject(float width, float height) : m_width(width), m_height(height) {}

  // IDWriteInlineObject methods
  STDMETHOD(Draw)
  (_In_opt_ void *clientDrawingContext,
   _In_ IDWriteTextRenderer *renderer,
   FLOAT originX,
   FLOAT originY,
   BOOL isSideways,
   BOOL isRightToLeft,
   _In_opt_ IUnknown *clientDrawingEffect) override {
    // We don't need to draw anything here since the actual rendering is handled by React Native
    return S_OK;
  }

  STDMETHOD(GetMetrics)(_Out_ DWRITE_INLINE_OBJECT_METRICS *metrics) override {
    metrics->width = m_width;
    metrics->height = m_height;
    metrics->baseline =
        m_height; // If the baseline is at the bottom, then baseline = height
                  // (https://learn.microsoft.com/en-us/windows/win32/api/dwrite/ns-dwrite-dwrite_inline_object_metrics)
    metrics->supportsSideways = true;
    return S_OK;
  }

  STDMETHOD(GetOverhangMetrics)(_Out_ DWRITE_OVERHANG_METRICS *overhangs) override {
    overhangs->left = 0;
    overhangs->top = 0;
    overhangs->right = 0;
    overhangs->bottom = 0;
    return S_OK;
  }

  STDMETHOD(GetBreakConditions)
  (_Out_ DWRITE_BREAK_CONDITION *breakConditionBefore, _Out_ DWRITE_BREAK_CONDITION *breakConditionAfter) override {
    *breakConditionBefore = DWRITE_BREAK_CONDITION_NEUTRAL;
    *breakConditionAfter = DWRITE_BREAK_CONDITION_NEUTRAL;
    return S_OK;
  }

 private:
  float m_width;
  float m_height;
};

void TextLayoutManager::GetTextLayout(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    Size size,
    winrt::com_ptr<IDWriteTextLayout> &spTextLayout,
    TextMeasurement::Attachments &attachments) noexcept {
  const auto &attributedString = attributedStringBox.getValue();
  auto fragments = attributedString.getFragments();
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
      (outerFragment.textAttributes.allowFontScaling.value_or(true) &&
       !std::isnan(outerFragment.textAttributes.fontSizeMultiplier))
          ? (outerFragment.textAttributes.fontSizeMultiplier * outerFragment.textAttributes.fontSize)
          : outerFragment.textAttributes.fontSize,
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

  // Set text alignment
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

  // Get text with Object Replacement Characters for attachments
  auto str = GetTransformedText(attributedStringBox);

  winrt::check_hresult(Microsoft::ReactNative::DWriteFactory()->CreateTextLayout(
      str.c_str(), // The string to be laid out and formatted.
      static_cast<UINT32>(str.size()), // The length of the string.
      spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
      size.width, // The width of the layout box.
      size.height, // The height of the layout box.
      spTextLayout.put() // The IDWriteTextLayout interface pointer.
      ));

  // Calculate positions for attachments and set inline objects
  unsigned int position = 0;
  for (const auto &fragment : fragments) {
    if (fragment.isAttachment()) {
      // Get attachment dimensions
      float width = fragment.parentShadowView.layoutMetrics.frame.size.width;
      float height = fragment.parentShadowView.layoutMetrics.frame.size.height;

      // Get text metrics to check for clipping
      DWRITE_TEXT_METRICS dtm{};
      winrt::check_hresult(spTextLayout->GetMetrics(&dtm));

      // Check if the attachment should be clipped
      // TODO #14443: clipping works on the first-levels view, but any nested view won't be clipped
      bool isClipped = height > dtm.height;
      if (isClipped) {
        height = dtm.height;
      }

      // Create an inline object with the correct dimensions
      auto inlineObject = winrt::make<AttachmentInlineObject>(width, height);

      // Set the inline object at this position
      winrt::check_hresult(spTextLayout->SetInlineObject(inlineObject.get(), {position, 1}));

      // Get the position of the Object Replacement Character
      DWRITE_HIT_TEST_METRICS hitTestMetrics;
      float x, y;
      winrt::check_hresult(spTextLayout->HitTestTextPosition(position, false, &x, &y, &hitTestMetrics));

      // Store the attachment position and dimensions
      TextMeasurement::Attachment attachment;
      attachment.frame = {
          x, // left
          y, // top
          width, // width
          height // height
      };
      attachment.isClipped = isClipped;
      attachments.push_back(attachment);
      position += 1;
    } else {
      unsigned int length = static_cast<UINT32>(fragment.string.length());
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
      winrt::check_hresult(spTextLayout->SetFontSize(
          (attributes.allowFontScaling.value_or(true) && !std::isnan(attributes.fontSizeMultiplier))
              ? (attributes.fontSizeMultiplier * attributes.fontSize)
              : attributes.fontSize,
          range));

      if (!isnan(attributes.letterSpacing)) {
        winrt::check_hresult(
            spTextLayout.as<IDWriteTextLayout1>()->SetCharacterSpacing(0, attributes.letterSpacing, 0, range));
      }
      position += length;
    }
  }
}

void TextLayoutManager::GetTextLayout(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    LayoutConstraints layoutConstraints,
    winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept {
  if (attributedStringBox.getValue().isEmpty())
    return;

  TextMeasurement::Attachments attachments;
  GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints.maximumSize, spTextLayout, attachments);
}

// measure entire text (inluding attachments)
TextMeasurement TextLayoutManager::measure(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    const TextLayoutContext &layoutContext,
    LayoutConstraints layoutConstraints) const {
  TextMeasurement measurement{};
  auto &attributedString = attributedStringBox.getValue();

  measurement = m_measureCache.get(
      {attributedString, paragraphAttributes, layoutConstraints}, [&](TextMeasureCacheKey const &key) {
        auto telemetry = TransactionTelemetry::threadLocalTelemetry();
        if (telemetry) {
          telemetry->willMeasureText();
        }

        winrt::com_ptr<IDWriteTextLayout> spTextLayout;

        TextMeasurement::Attachments attachments;
        GetTextLayout(
            attributedStringBox, paragraphAttributes, layoutConstraints.maximumSize, spTextLayout, attachments);

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
          measurement.attachments = attachments;
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
    const ParagraphAttributes &paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  assert(false);
  return {};
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

LinesMeasurements TextLayoutManager::measureLines(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    Size size) const {
  LinesMeasurements lineMeasurements{};

  winrt::com_ptr<IDWriteTextLayout> spTextLayout;
  TextMeasurement::Attachments attachments;
  GetTextLayout(attributedStringBox, paragraphAttributes, size, spTextLayout, attachments);

  if (spTextLayout) {
    std::vector<DWRITE_LINE_METRICS> lineMetrics;
    uint32_t actualLineCount;
    spTextLayout->GetLineMetrics(nullptr, 0, &actualLineCount);
    lineMetrics.resize(static_cast<size_t>(actualLineCount));
    winrt::check_hresult(spTextLayout->GetLineMetrics(lineMetrics.data(), actualLineCount, &actualLineCount));
    uint32_t startRange = 0;
    const auto count = (paragraphAttributes.maximumNumberOfLines > 0)
        ? std::min(static_cast<uint32_t>(paragraphAttributes.maximumNumberOfLines), actualLineCount)
        : actualLineCount;
    for (uint32_t i = 0; i < count; ++i) {
      UINT32 actualHitTestCount = 0;
      spTextLayout->HitTestTextRange(
          startRange,
          lineMetrics[i].length,
          0, // x
          0, // y
          NULL,
          0, // metrics count
          &actualHitTestCount);

      // Allocate enough room to return all hit-test metrics.
      std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);
      spTextLayout->HitTestTextRange(
          startRange,
          lineMetrics[i].length,
          0, // x
          0, // y
          &hitTestMetrics[0],
          static_cast<UINT32>(hitTestMetrics.size()),
          &actualHitTestCount);

      float width = 0;
      for (auto tm : hitTestMetrics) {
        width += tm.width;
      }

      std::string str;
      const auto &attributedString = attributedStringBox.getValue();
      for (const auto &fragment : attributedString.getFragments()) {
        str = str +
            winrt::to_string(Microsoft::ReactNative::TransformableText::TransformText(
                winrt::hstring{Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string)},
                ConvertTextTransform(fragment.textAttributes.textTransform)));
      }

      lineMeasurements.emplace_back(LineMeasurement(
          str.substr(startRange, lineMetrics[i].length),
          {{hitTestMetrics[0].left, hitTestMetrics[0].top}, // origin
           {width, lineMetrics[i].height}},
          0.0f, // TODO descender
          0.0f, // TODO: capHeight
          0.0f, // TODO ascender
          0.0f // TODO: xHeight
          ));

      startRange += lineMetrics[i].length;
    }
  }

  return lineMeasurements;
}

std::shared_ptr<void> TextLayoutManager::getHostTextStorage(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  return nullptr;
}

void *TextLayoutManager::getNativeTextLayoutManager() const {
  return (void *)this;
}

Float TextLayoutManager::baseline(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    Size size) const {
  winrt::com_ptr<IDWriteTextLayout> spTextLayout;
  TextMeasurement::Attachments attachments;
  GetTextLayout(attributedStringBox, paragraphAttributes, size, spTextLayout, attachments);

  if (!spTextLayout) {
    return 0;
  }

  DWRITE_TEXT_METRICS metrics;
  winrt::check_hresult(spTextLayout->GetMetrics(&metrics));
  return metrics.height * 0.8f;
}

winrt::hstring TextLayoutManager::GetTransformedText(const AttributedStringBox &attributedStringBox) {
  winrt::hstring result{};
  const auto &attributedString = attributedStringBox.getValue();

  for (const auto &fragment : attributedString.getFragments()) {
    if (fragment.isAttachment()) {
      result = result + L"\uFFFC"; // Unicode Object Replacement Character, will be replaced with an inline object
    } else {
      result = result +
          Microsoft::ReactNative::TransformableText::TransformText(
                   winrt::hstring{Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string)},
                   ConvertTextTransform(fragment.textAttributes.textTransform));
    }
  }
  return result;
}

} // namespace facebook::react
