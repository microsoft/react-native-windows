// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <Fabric/DWriteHelpers.h>
#include <Utils/TransformableText.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <react/renderer/telemetry/TransactionTelemetry.h>
#include "WindowsTextLayoutManager.h"

#include <unicode.h>

constexpr float cDefaultMaxFontSizeMultiplier = 0.0f;

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

TextLayoutManager::TextLayoutManager(const std::shared_ptr<const ContextContainer> &contextContainer)
    : contextContainer_(contextContainer), textMeasureCache_(kSimpleThreadSafeCacheSizeCap) {}

WindowsTextLayoutManager::WindowsTextLayoutManager(const std::shared_ptr<const ContextContainer> &contextContainer)
    : TextLayoutManager(contextContainer) {}

void WindowsTextLayoutManager::GetTextLayout(
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

  float fontSizeText = outerFragment.textAttributes.fontSize;
  if (outerFragment.textAttributes.allowFontScaling.value_or(true) &&
      !std::isnan(outerFragment.textAttributes.fontSizeMultiplier)) {
    float maxFontSizeMultiplierText = cDefaultMaxFontSizeMultiplier;
    maxFontSizeMultiplierText =
        (!std::isnan(outerFragment.textAttributes.maxFontSizeMultiplier)
             ? outerFragment.textAttributes.maxFontSizeMultiplier
             : cDefaultMaxFontSizeMultiplier);
    fontSizeText *= (maxFontSizeMultiplierText >= 1.0f)
        ? std::min(maxFontSizeMultiplierText, outerFragment.textAttributes.fontSizeMultiplier)
        : outerFragment.textAttributes.fontSizeMultiplier;
  }

  winrt::check_hresult(Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
      outerFragment.textAttributes.fontFamily.empty()
          ? L"Segoe UI"
          : Microsoft::Common::Unicode::Utf8ToUtf16(outerFragment.textAttributes.fontFamily).c_str(),
      nullptr, // Font collection (nullptr sets it to use the system font collection).
      static_cast<DWRITE_FONT_WEIGHT>(outerFragment.textAttributes.fontWeight.value_or(
          static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
      style,
      DWRITE_FONT_STRETCH_NORMAL,
      fontSizeText,
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

  // Apply max width constraint and ellipsis trimming to ensure consistency with rendering
  DWRITE_TEXT_METRICS metrics;
  winrt::check_hresult(spTextLayout->GetMetrics(&metrics));

  if (metrics.width > size.width) {
    spTextLayout->SetMaxWidth(size.width);
  }

  // Apply DWRITE_TRIMMING for ellipsizeMode
  DWRITE_TRIMMING trimming = {};
  winrt::com_ptr<IDWriteInlineObject> ellipsisSign;

  switch (paragraphAttributes.ellipsizeMode) {
    case facebook::react::EllipsizeMode::Tail:
      trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
      break;
    case facebook::react::EllipsizeMode::Clip:
      trimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
      break;
    default:
      trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER; // Default to tail behavior
      break;
  }

  // Use DWriteFactory to create the ellipsis trimming sign
  if (trimming.granularity != DWRITE_TRIMMING_GRANULARITY_NONE) {
    auto dwriteFactory = Microsoft::ReactNative::DWriteFactory();
    HRESULT hr = dwriteFactory->CreateEllipsisTrimmingSign(spTextLayout.get(), ellipsisSign.put());
    if (SUCCEEDED(hr)) {
      spTextLayout->SetTrimming(&trimming, ellipsisSign.get());
    }
  }

  // Calculate positions for attachments and set inline objects
  unsigned int position = 0;
  for (const auto &fragment : fragments) {
    if (fragment.isAttachment()) {
      float width = fragment.parentShadowView.layoutMetrics.frame.size.width;
      float height = fragment.parentShadowView.layoutMetrics.frame.size.height;

      // Get current height to check if attachment needs to be clipped
      DWRITE_TEXT_METRICS dtm{};
      winrt::check_hresult(spTextLayout->GetMetrics(&dtm));

      // Check if the attachment should be clipped
      // TODO #14443: clipping works on the first-levels view, but any nested view won't be clipped
      bool isClipped = height > dtm.height;
      if (isClipped) {
        height = dtm.height;
      }

      // Create an inline object (this just reserves space in RichEdit for ReactNative to render the actual attachment)
      auto inlineObject = winrt::make<AttachmentInlineObject>(width, height);
      winrt::check_hresult(spTextLayout->SetInlineObject(inlineObject.get(), {position, 1}));

      // Get the position of the Object Replacement Character
      DWRITE_HIT_TEST_METRICS hitTestMetrics;
      float x, y;
      winrt::check_hresult(spTextLayout->HitTestTextPosition(position, false, &x, &y, &hitTestMetrics));

      // Store the attachment position for RN to render later
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

      float maxFontSizeMultiplier = cDefaultMaxFontSizeMultiplier;
      maxFontSizeMultiplier =
          (!std::isnan(attributes.maxFontSizeMultiplier) ? attributes.maxFontSizeMultiplier
                                                         : cDefaultMaxFontSizeMultiplier);
      float fontSize = attributes.fontSize;
      if (attributes.allowFontScaling.value_or(true) && (!std::isnan(attributes.fontSizeMultiplier))) {
        fontSize *= (maxFontSizeMultiplier >= 1.0f) ? std::min(maxFontSizeMultiplier, attributes.fontSizeMultiplier)
                                                    : attributes.fontSizeMultiplier;
      }

      winrt::check_hresult(spTextLayout->SetFontSize(fontSize, range));

      if (!isnan(attributes.letterSpacing)) {
        winrt::check_hresult(
            spTextLayout.as<IDWriteTextLayout1>()->SetCharacterSpacing(0, attributes.letterSpacing, 0, range));
      }
      position += length;
    }
  }
}

void WindowsTextLayoutManager::GetTextLayout(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    LayoutConstraints layoutConstraints,
    winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept {
  if (attributedStringBox.getValue().isEmpty())
    return;

  TextMeasurement::Attachments attachments;
  if (paragraphAttributes.adjustsFontSizeToFit) {
    auto minimumFontScale = 0.01f;
    // Uncomment below part when minimumFontScale is available in ParagraphAttributes
    // if (paragraphAttributes.minimumFontScale && paragraphAttributes.minimumFontScale >= 0.01f &&
    //    paragraphAttributes.minimumFontScale <= 1.0f) {
    //  minimumFontScale = paragraphAttributes.minimumFontScale;
    //}
    GetTextLayoutByAdjustingFontSizeToFit(
        attributedStringBox, paragraphAttributes, layoutConstraints, spTextLayout, attachments, minimumFontScale);
  } else {
    GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints.maximumSize, spTextLayout, attachments);
  }
}

void WindowsTextLayoutManager::GetTextLayoutByAdjustingFontSizeToFit(
    AttributedStringBox attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    LayoutConstraints layoutConstraints,
    winrt::com_ptr<IDWriteTextLayout> &spTextLayout,
    TextMeasurement::Attachments &attachments,
    float minimumFontScale) noexcept {
  /* This function constructs a text layout from the given parameters.
  If the generated text layout doesn't fit within the given layout constraints,
  it will reduce the font size and construct a new text layout. This process will
  be repeated until the text layout meets the constraints.*/

  DWRITE_TEXT_METRICS metrics;

  // Better Approach should be implemented, this uses O(n)
  constexpr auto fontReduceFactor = 1.0f;
  auto attributedStringToResize = attributedStringBox.getValue();
  auto fragmentsCopyToResize = attributedStringToResize.getFragments();
  if (fragmentsCopyToResize.empty()) {
    return; // No fragments to process
  }

  float initialFontSize = fragmentsCopyToResize[0].textAttributes.fontSize;
  float currentFontSize = initialFontSize;

  // Calculate the minimum font size as per Android/IOS
  float minimumFontSize = std::max(minimumFontScale * initialFontSize, 4.0f);

  // Initial measurement
  GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints.maximumSize, spTextLayout, attachments);
  if (spTextLayout) {
    winrt::check_hresult(spTextLayout->GetMetrics(&metrics));
  } else {
    return;
  }

  // Loop until the font size is reduced to the minimum or the layout fits
  while ((currentFontSize > minimumFontSize) &&
         ((paragraphAttributes.maximumNumberOfLines != 0 &&
           paragraphAttributes.maximumNumberOfLines < static_cast<int>(metrics.lineCount)) ||
          metrics.height > metrics.layoutHeight || metrics.width > metrics.layoutWidth)) {
    // Reduce the font size by 1 point (or a configurable factor)
    currentFontSize = std::max(currentFontSize - fontReduceFactor, minimumFontSize);

    // Adjust font size for all fragments proportionally
    attributedStringToResize.getFragments().clear();
    for (auto fragment : fragmentsCopyToResize) {
      fragment.textAttributes.fontSize =
          std::max(fragment.textAttributes.fontSize * (currentFontSize / initialFontSize), minimumFontSize);
      attributedStringToResize.appendFragment(std::move(fragment));
    }

    attributedStringBox = facebook::react::AttributedStringBox(attributedStringToResize);

    // Re-measure the text layout
    GetTextLayout(attributedStringBox, paragraphAttributes, layoutConstraints.maximumSize, spTextLayout, attachments);
    if (spTextLayout) {
      winrt::check_hresult(spTextLayout->GetMetrics(&metrics));
    } else {
      return;
    }
  }
}

// measure entire text (inluding attachments)
TextMeasurement TextLayoutManager::measure(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    const TextLayoutContext &layoutContext,
    const LayoutConstraints &layoutConstraints) const {
  TextMeasurement measurement{};
  auto &attributedString = attributedStringBox.getValue();

  measurement = textMeasureCache_.get(
      {attributedString, paragraphAttributes, layoutConstraints}, [&](TextMeasureCacheKey const &key) {
        auto telemetry = TransactionTelemetry::threadLocalTelemetry();
        if (telemetry) {
          telemetry->willMeasureText();
        }

        winrt::com_ptr<IDWriteTextLayout> spTextLayout;

        TextMeasurement::Attachments attachments;
        WindowsTextLayoutManager::GetTextLayout(
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

winrt::hstring WindowsTextLayoutManager::GetTransformedText(const AttributedStringBox &attributedStringBox) {
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
