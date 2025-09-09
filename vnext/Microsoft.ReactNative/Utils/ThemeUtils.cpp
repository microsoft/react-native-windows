// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <Utils/ThemeUtils.h>

#include <react/renderer/graphics/Color.h>
#include <react/renderer/graphics/HostPlatformColor.h>
#include <winuser.h>

namespace Microsoft::ReactNative {

bool IsColorLight(const winrt::Windows::UI::Color &clr) noexcept {
  return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
}

bool IsInHighContrastWin32() noexcept {
  BOOL fResult;
  HIGHCONTRAST info;

  fResult = SystemParametersInfo(
      SPI_GETHIGHCONTRAST, // Get contrast information
      0, // Not used
      &info, // Holds contrast information
      0); // Not used

  // Double it.
  if (fResult) {
    return !!(info.dwFlags & HCF_HIGHCONTRASTON);
  }

  return false;
}

int CalculateColorBrightness(const winrt::Windows::UI::Color &color) noexcept {
  return CalculateColorBrightness(color.R, color.G, color.B);
}

int CalculateColorBrightness(int r, int g, int b) noexcept {
  return (r * kColorBrightnessRedWeight + g * kColorBrightnessGreenWeight + b * kColorBrightnessBlueWeight) /
      kColorBrightnessDivisor;
}

facebook::react::SharedColor GetCaretColor(
    const facebook::react::SharedColor &cursorColor,
    const facebook::react::SharedColor &foregroundColor,
    const facebook::react::SharedColor &backgroundColor) noexcept {
  const auto defaultCaretColor =
      facebook::react::hostPlatformColorFromRGBA(0, 0, 0, 0xFF); // Default caret color is black

  if (cursorColor) {
    return cursorColor;
  } else if (foregroundColor) {
    // Extra Caution if Background color is present
    auto fgWindows = (*foregroundColor).AsWindowsColor();
    int fgBrightness = CalculateColorBrightness(fgWindows);

    // If foreground is very light and background is also very light, force black caret.
    if (fgBrightness > kCaretLightForegroundThreshold && facebook::react::isColorMeaningful(backgroundColor)) {
      auto bgWindows = (*backgroundColor).AsWindowsColor();
      int bgBrightness = CalculateColorBrightness(bgWindows);
      if (bgBrightness > kCaretLightBackgroundThreshold) {
        // Use opaque black caret
        return defaultCaretColor;
      }
    }

    return foregroundColor;
  } else {
    return defaultCaretColor;
  }
}

} // namespace Microsoft::ReactNative
