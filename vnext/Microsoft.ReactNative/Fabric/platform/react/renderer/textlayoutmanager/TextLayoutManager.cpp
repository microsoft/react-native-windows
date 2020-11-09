// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include "TextLayoutManager.h"

#include <dwrite.h>

#include <unicode.h>
#include <wingdi.h>

namespace facebook {
namespace react {

TextLayoutManager::~TextLayoutManager() {}

// TODO: This function is inefficient (not caching anything), and only handles single strings.
TextMeasurement TextLayoutManager::measure(
      AttributedStringBox attributedStringBox,
      ParagraphAttributes paragraphAttributes,
      LayoutConstraints layoutConstraints) const {

  assert(attributedStringBox.getValue().getFragments().size() == 1);

  IDWriteFactory *pDWriteFactory;
  DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&pDWriteFactory));

  for (auto &fragment : attributedStringBox.getValue().getFragments()) {

  IDWriteTextFormat *pTextFormat;
    pDWriteFactory->CreateTextFormat(
        L"Gabriola", // Font family name.
        NULL, // Font collection (NULL sets it to use the system font collection).
        static_cast<DWRITE_FONT_WEIGHT>(fragment.textAttributes.fontWeight.value_or(static_cast<facebook::react::FontWeight>(DWRITE_FONT_WEIGHT_REGULAR))),
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fragment.textAttributes.fontSize,
        L"en-us",
        &pTextFormat);

    auto str = Microsoft::Common::Unicode::Utf8ToUtf16(fragment.string);

    IDWriteTextLayout *pTextLayout;
    pDWriteFactory->CreateTextLayout(
        str.c_str(), // The string to be laid out and formatted.
        static_cast<UINT32>(str.length()), // The length of the string.
        pTextFormat, // The text format to apply to the string (contains font information, etc).
        layoutConstraints.maximumSize.width, // The width of the layout box.
        layoutConstraints.maximumSize.height, // The height of the layout box.
        &pTextLayout // The IDWriteTextLayout interface pointer.
    );

    TextMeasurement tm;

    DWRITE_TEXT_METRICS dtm;
    pTextLayout->GetMetrics(&dtm);
    tm.size = {dtm.width, dtm.height};
    return tm;
  }

  /*
  
        // Get line bounds
  float baseline;
  float lineAdvance;
  float lineStackingOffset;
  IFC_RETURN(GetLineHeight(&baseline, &lineAdvance));

  // Fast path for empty TextBlock, always return the line height.
  if (m_strText.GetCount() == 0) {
    IFC_RETURN(GetLineStackingOffset(1, &lineStackingOffset)); // Empty TextBlock is always formatted into 1 line.
    desiredSize.width = m_padding.left + m_padding.right;
    desiredSize.height = lineAdvance - lineStackingOffset + m_padding.top + m_padding.bottom;
    return S_OK;
  }

  availableSize.width -= m_padding.left + m_padding.right;
  availableSize.height -= m_padding.top + m_padding.bottom;
  availableSize.width = MAX(availableSize.width, 0);
  availableSize.height = MAX(availableSize.height, 0);

  IFC_RETURN(ConfigureDWriteTextLayout(availableSize, baseline, lineAdvance));

  ASSERT(m_pTextLayout);

  DWRITE_TEXT_METRICS textMetrics = {};
  IFC_RETURN(m_pTextLayout->GetMetrics(&textMetrics));

  // hack for MaxLines Property. Calcuate total line height for m_maxLines lines, then trim it.
  if (m_maxLines != 0 && m_maxLines < textMetrics.lineCount) {
    uint32_t actualLineCount = 0;
    float maxHeight = 0;

    std::vector<DWRITE_LINE_METRICS> lineInformation(textMetrics.lineCount);

    IFC_RETURN(m_pTextLayout->GetLineMetrics(lineInformation.data(), textMetrics.lineCount, &actualLineCount));
    for (uint32_t index = 0; index < m_maxLines; index++) {
      maxHeight += lineInformation[index].height;
    }

    IFC_RETURN(m_pTextLayout->SetMaxHeight(maxHeight));

    // When MaxLines is smaller than the actual line count, we need to ask DWriteTextLayout to trim the text if
    // TextTrimming == DirectUI::TextTrimming::None. The default granularity is word.
    if (m_textTrimming == DirectUI::TextTrimming::None) {
      DWRITE_TRIMMING trimmingOptions;
      trimmingOptions = {
          DWRITE_TRIMMING_GRANULARITY_CHARACTER,
          0, // delimiter
          0 // delimiter occurence
      };

      IFC_RETURN(m_pTextLayout->SetTrimming(&trimmingOptions, nullptr));
    }

    IFC_RETURN(m_pTextLayout->GetMetrics(&textMetrics));
  }

  // Calculate the bottom adjustment for LineStackingStrategy.BaselineToBaseline && LineHeight  > 0.
  IFC_RETURN(GetLineStackingOffset(textMetrics.lineCount, &lineStackingOffset));
  desiredSize.width = textMetrics.width + m_padding.left + m_padding.right;
  desiredSize.height = textMetrics.height - lineStackingOffset + m_padding.top + m_padding.bottom;
  */
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


void * TextLayoutManager::getNativeTextLayoutManager() const {
  return (void*)this;
}

} // namespace react
} // namespace facebook
