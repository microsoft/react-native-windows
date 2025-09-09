// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/graphics/Color.h>
#include <winrt/Windows.UI.h>

namespace Microsoft::ReactNative {

// Color brightness calculation constants (standard luminance formula)
constexpr int kColorBrightnessRedWeight = 299;
constexpr int kColorBrightnessGreenWeight = 587;
constexpr int kColorBrightnessBlueWeight = 114;
constexpr int kColorBrightnessDivisor = 1000;

// Caret color threshold constants
constexpr int kCaretLightForegroundThreshold = 240;
constexpr int kCaretLightBackgroundThreshold = 186;
constexpr int kCaretSelectionBrightnessThreshold = 125;

bool IsColorLight(const winrt::Windows::UI::Color &clr) noexcept;
bool IsInHighContrastWin32() noexcept;

// Calculate brightness using standard luminance formula
int CalculateColorBrightness(const winrt::Windows::UI::Color &color) noexcept;

// Calculate brightness from RGB values
int CalculateColorBrightness(int r, int g, int b) noexcept;

// Determine appropriate caret color based on foreground and background colors
facebook::react::SharedColor GetCaretColor(
    const facebook::react::SharedColor &cursorColor,
    const facebook::react::SharedColor &foregroundColor,
    const facebook::react::SharedColor &backgroundColor) noexcept;

} // namespace Microsoft::ReactNative
