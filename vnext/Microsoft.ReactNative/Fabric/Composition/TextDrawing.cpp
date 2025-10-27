
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "TextDrawing.h"

#include <AutoDraw.h>
#include <Fabric/platform/react/renderer/graphics/PlatformColorUtils.h>
#include <Utils/ValueUtils.h>
#include <unicode.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition {

void RenderText(
    ID2D1RenderTarget &deviceContext,
    ::IDWriteTextLayout &textLayout,
    const facebook::react::AttributedString &attributedString,
    const facebook::react::TextAttributes &textAttributes,
    facebook::react::Point offset,
    float pointScaleFactor,
    winrt::Microsoft::ReactNative::Composition::implementation::Theme &theme) noexcept {
  float offsetX = offset.x / pointScaleFactor;
  float offsetY = offset.y / pointScaleFactor;

  const auto dpi = pointScaleFactor * 96.0f;
  float oldDpiX, oldDpiY;
  deviceContext.GetDpi(&oldDpiX, &oldDpiY);
  deviceContext.SetDpi(dpi, dpi);

  // Create a solid color brush for the text. A more sophisticated application might want
  // to cache and reuse a brush across all text elements instead, taking care to recreate
  // it in the event of device removed.
  winrt::com_ptr<ID2D1SolidColorBrush> brush;

  // Check if we should use theme-aware default color instead of hardcoded black
  bool useDefaultColor = false;
  if (textAttributes.foregroundColor) {
    auto &color = *textAttributes.foregroundColor;
    // If it's black (or very dark) without explicit PlatformColor, use theme-aware color
    if (color.m_platformColor.empty() && color.m_color.R <= 10 && color.m_color.G <= 10 && color.m_color.B <= 10) {
      useDefaultColor = true;
    }
  } else {
    useDefaultColor = true;
  }

  if (useDefaultColor) {
    // Use theme-aware TextFillColorPrimary which adapts to light/dark mode
    auto d2dColor = theme.D2DPlatformColor("TextFillColorPrimary");
    winrt::check_hresult(deviceContext.CreateSolidColorBrush(d2dColor, brush.put()));
  } else {
    // User set explicit color or PlatformColor - use it
    auto color = theme.D2DColor(*textAttributes.foregroundColor);
    winrt::check_hresult(deviceContext.CreateSolidColorBrush(color, brush.put()));
  }

  if (textAttributes.textDecorationLineType) {
    DWRITE_TEXT_RANGE range = {0, std::numeric_limits<uint32_t>::max()};
    if (*(textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::Underline ||
        *(textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
      textLayout.SetUnderline(true, range);
    } else {
      textLayout.SetUnderline(false, range);
    }
  }

  if (textAttributes.textDecorationLineType) {
    DWRITE_TEXT_RANGE range = {0, std::numeric_limits<uint32_t>::max()};
    if (*(textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::Strikethrough ||
        *(textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
      textLayout.SetStrikethrough(true, range);
    } else {
      textLayout.SetStrikethrough(false, range);
    }
  }

  // Create color effects for individual text fragments.
  unsigned int position = 0;
  unsigned int length = 0;
  for (auto fragment : attributedString.getFragments()) {
    length = static_cast<UINT32>(fragment.string.length());
    DWRITE_TEXT_RANGE range = {position, length};
    if (fragment.textAttributes.foregroundColor &&
            (fragment.textAttributes.foregroundColor != textAttributes.foregroundColor) ||
        !isnan(fragment.textAttributes.opacity)) {
      winrt::com_ptr<ID2D1SolidColorBrush> fragmentBrush;

      // Check if we should use theme-aware default color for this fragment
      bool useFragmentDefaultColor = false;
      if (fragment.textAttributes.foregroundColor) {
        auto &color = *fragment.textAttributes.foregroundColor;
        // If it's black (or very dark) without explicit PlatformColor, use theme-aware color
        if (color.m_platformColor.empty() && color.m_color.R <= 10 && color.m_color.G <= 10 && color.m_color.B <= 10) {
          useFragmentDefaultColor = true;
        }
      } else {
        useFragmentDefaultColor = true;
      }

      if (useFragmentDefaultColor) {
        // Use theme-aware TextFillColorPrimary which adapts to light/dark mode
        auto d2dColor = theme.D2DPlatformColor("TextFillColorPrimary");
        winrt::check_hresult(deviceContext.CreateSolidColorBrush(d2dColor, fragmentBrush.put()));
      } else {
        // User set explicit color or PlatformColor - use it
        auto color = theme.D2DColor(*fragment.textAttributes.foregroundColor);
        winrt::check_hresult(deviceContext.CreateSolidColorBrush(color, fragmentBrush.put()));
      }

      if (fragment.textAttributes.textDecorationLineType) {
        if (*(fragment.textAttributes.textDecorationLineType) == facebook::react::TextDecorationLineType::Underline ||
            *(fragment.textAttributes.textDecorationLineType) ==
                facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
          textLayout.SetUnderline(true, range);
        } else {
          textLayout.SetUnderline(false, range);
        }
      }

      if (fragment.textAttributes.textDecorationLineType) {
        if (*(fragment.textAttributes.textDecorationLineType) ==
                facebook::react::TextDecorationLineType::Strikethrough ||
            *(fragment.textAttributes.textDecorationLineType) ==
                facebook::react::TextDecorationLineType::UnderlineStrikethrough) {
          textLayout.SetStrikethrough(true, range);
        } else {
          textLayout.SetStrikethrough(false, range);
        }
      }

      if (!isnan(fragment.textAttributes.opacity)) {
        fragmentBrush->SetOpacity(fragment.textAttributes.opacity);
      }
      textLayout.SetDrawingEffect(fragmentBrush.get(), range);

      // DWrite doesn't handle background highlight colors, so we manually draw the background color for ranges
      if (facebook::react::isColorMeaningful(fragment.textAttributes.backgroundColor)) {
        UINT32 actualHitTestCount = 0;
        if (range.length > 0) {
          textLayout.HitTestTextRange(
              range.startPosition,
              range.length,
              0, // x
              0, // y
              NULL,
              0, // metrics count
              &actualHitTestCount);
        }

        // Allocate enough room to return all hit-test metrics.
        std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTestCount);
        if (range.length > 0) {
          textLayout.HitTestTextRange(
              range.startPosition,
              range.length,
              0, // x
              0, // y
              &hitTestMetrics[0],
              static_cast<UINT32>(hitTestMetrics.size()),
              &actualHitTestCount);
        }

        // Draw the selection ranges behind the text.
        if (actualHitTestCount > 0) {
          // Note that an ideal layout will return fractional values,
          // so you may see slivers between the selection ranges due
          // to the per-primitive anti-aliasing of the edges unless
          // it is disabled (better for performance anyway).
          auto oldAliasMode = deviceContext.GetAntialiasMode();
          deviceContext.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

          winrt::com_ptr<ID2D1SolidColorBrush> textHighlightBrush;
          winrt::check_hresult(deviceContext.CreateSolidColorBrush(
              theme.D2DColor(*fragment.textAttributes.backgroundColor), textHighlightBrush.put()));

          for (size_t i = 0; i < actualHitTestCount; ++i) {
            const DWRITE_HIT_TEST_METRICS &htm = hitTestMetrics[i];

            const D2D1_RECT_F rect = {
                std::round(htm.left + offsetX),
                std::round(htm.top + offsetY),
                std::round(htm.left + htm.width + offsetX),
                std::round(htm.top + htm.height + offsetY)};

            deviceContext.FillRectangle(rect, textHighlightBrush.get());
          }
          deviceContext.SetAntialiasMode(oldAliasMode);
        }
      }
    }

    position += length;
  }

  // Draw the line of text at the specified offset, which corresponds to the top-left
  // corner of our drawing surface. Notice we don't call BeginDraw on the D2D device
  // context; this has already been done for us by the composition API.
  deviceContext.DrawTextLayout(
      D2D1::Point2F(offsetX, offsetY), &textLayout, brush.get(), D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

  // restore dpi to old state
  deviceContext.SetDpi(oldDpiX, oldDpiY);
}

} // namespace winrt::Microsoft::ReactNative::Composition
