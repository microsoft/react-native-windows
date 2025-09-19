// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.h>
#ifdef USE_FABRIC
#include <react/renderer/graphics/Color.h>
#endif

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

#ifdef USE_FABRIC
// Check if a color is meaningful (handles both PAPER and Fabric architectures)
bool isColorMeaningful(const facebook::react::SharedColor &color) noexcept;

// Determine appropriate caret color based on foreground and background colors
facebook::react::SharedColor GetCaretColor(
    const facebook::react::SharedColor &cursorColor,
    const facebook::react::SharedColor &foregroundColor,
    const facebook::react::SharedColor &backgroundColor) noexcept;
#endif

} // namespace Microsoft::ReactNative
