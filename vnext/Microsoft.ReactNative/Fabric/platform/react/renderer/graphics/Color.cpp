// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Color.h"
#include <Utils/ValueUtils.h>

namespace facebook::react {

bool isColorMeaningful(SharedColor const &color) noexcept {
  if (!color) {
    return false;
  }

  return colorComponentsFromColor(color).alpha > 0;
}

struct string_hash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const char* txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(std::string_view txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(const std::string& txt) const {
    return std::hash<std::string>{}(txt);
  }
};

winrt::Windows::UI::Color ResolvePlatformColor(Color const * const color) {
  // These are all the light-theme values. Which is better than no values
  // Arguably only the right-most platform colors should be respected, and 
  // Button should be updated to use those instead, assuming that still 
  // holds up in high contrast and such.
  static std::unordered_map<std::string, winrt::Windows::UI::Color, string_hash, std::equal_to<>> s_windowsColors = {
    {"SolidBackgroundFillColorBaseBrush", { 0xFF, 0xF3, 0xF3, 0xF3 }}, // SolidBackgroundFillColorBase
    {"ButtonBackgroundPressed", { 0x4D, 0xF9, 0xF9, 0xF9 }}, // ControlFillColorTertiary
    {"ButtonForegroundPressed", { 0x9E, 0x00, 0x00, 0x00 }}, // TextFillColorSecondary
    {"ButtonForegroundPointerOver", { 0xE4, 0x00, 0x00, 0x00 }}, // TextFillColorPrimaryBrush
    {"ButtonBackground", { 0xB3, 0xFF, 0xFF, 0xFF }}, // ControlFillColorDefault
    {"ButtonBorderBrush", { 0x29, 0x00, 0x00, 0x00 }}, // gradient from ControlStrokeColorSecondary to ControlStrokeColorDefault
    {"ButtonForeground", { 0xE4, 0x00, 0x00, 0x00 }}, // TextFillColorPrimary
    {"ButtonBackgroundDisabled", { 0x4D, 0xF9, 0xF9, 0xF9 }}, // ControlFillColorDisabled
    {"ButtonBorderBrushDisabled", { 0x0F, 0x00, 0x00, 0x00 }}, // ControlStrokeColorDefault
    {"ButtonForegroundDisabled", { 0x5C, 0x00, 0x00, 0x00 }}, // TextFillColorDisabled
    {"ButtonBackgroundPointerOver", { 0x80, 0xF9, 0xF9, 0xF9 }}, // ControlFillColorSecondary
    {"ButtonBorderBrushPointerOver", { 0x66, 0x00, 0x00, 0x00 }}, // gradient from ControlStrokeColorOnAccentSecondary to ControlStrokeColorOnAccentDefault
    {"ButtonBorderBrushPressed", { 0x00, 0xFF, 0xFF, 0xFF }}, // ControlFillColorTransparent
  };

  if (!color->m_platformColor.empty()) {
    auto result = s_windowsColors.find(color->m_platformColor);
    if (result != s_windowsColors.end()) {
      return result->second;
    }
    else {
      OutputDebugStringA(color->m_platformColor.c_str());
    }
  }

  return color->m_color;
}

D2D1::ColorF SharedColor::AsD2DColor() const {
  winrt::Windows::UI::Color color = ResolvePlatformColor(m_color.get());
  return {
      color.R / 255.0f,
      color.G / 255.0f,
      color.B / 255.0f,
      color.A / 255.0f };
}

winrt::Windows::UI::Color SharedColor::AsWindowsColor() const {
  return ResolvePlatformColor(m_color.get());
}

#ifndef CORE_ABI
xaml::Media::Brush SharedColor::AsWindowsBrush() const {
  if (!m_color)
    return nullptr;
  if (!m_color->m_platformColor.empty()) {
    return Microsoft::ReactNative::BrushFromColorObject(winrt::to_hstring(m_color->m_platformColor));
  }
  return xaml::Media::SolidColorBrush(m_color->m_color);
}
#endif // CORE_ABI

SharedColor colorFromComponents(ColorComponents components) {
  float ratio = 255;
  return {ui::ColorHelper::FromArgb(
      (int)round(components.alpha * ratio) & 0xff,
      (int)round(components.red * ratio) & 0xff,
      (int)round(components.green * ratio) & 0xff,
      (int)round(components.blue * ratio) & 0xff)};
}

ColorComponents colorComponentsFromColor(SharedColor const &sharedColor) {
  float ratio = 255;
  auto color = sharedColor.AsWindowsColor();
  return ColorComponents{
      (float)color.R / ratio, (float)color.G / ratio, (float)color.B / ratio, (float)color.A / ratio};
}

SharedColor clearColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 0});
  return color;
}

SharedColor blackColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 1});
  return color;
}

SharedColor whiteColor() {
  static SharedColor color = colorFromComponents(ColorComponents{1, 1, 1, 1});
  return color;
}

SharedColor greyColor() {
  static SharedColor color = colorFromComponents(ColorComponents{133, 133, 133, 1});
  return color;
}

} // namespace facebook::react
