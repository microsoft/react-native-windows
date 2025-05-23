// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <d2d1_1.h>
#include <react/renderer/graphics/ColorComponents.h>
#include "PlatformColorUtils.h"

namespace facebook::react {

struct Color {
  bool operator==(const Color &otherColor) const {
    return m_color == otherColor.m_color && m_platformColor == otherColor.m_platformColor;
  }
  bool operator!=(const Color &otherColor) const {
    return m_color != otherColor.m_color || m_platformColor != otherColor.m_platformColor;
  }

  winrt::Windows::UI::Color AsWindowsColor() const {
    if (m_platformColor.empty()) {
      return m_color;
    }
    return ResolvePlatformColor(m_platformColor);
  }

  COLORREF AsColorRefNoAlpha() const {
    return RGB(m_color.R, m_color.G, m_color.B);
  }

  COLORREF AsColorRefWithAlpha() const {
    return RGB(m_color.R, m_color.G, m_color.B) | (m_color.A << 24);
  }

  winrt::Windows::UI::Color m_color;
  std::vector<std::string> m_platformColor;
};

namespace HostPlatformColor {
static const facebook::react::Color UndefinedColor{
    {0, 0, 0, 0} /*Black*/,
    {"__undefinedColor"} /*Empty PlatformColors*/};
} // namespace HostPlatformColor

inline Color hostPlatformColorFromComponents(ColorComponents components) {
  float ratio = 255;
  const winrt::Windows::UI::Color color = {
      static_cast<uint8_t>((int)round(components.alpha * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.red * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.green * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.blue * ratio) & 0xff)};
  return {
      /* .m_color = */ color,
      /* .m_platformColor = */ {}};
}

inline Color hostPlatformColorFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  // Create ColorComponents from RGBA values normalized to the range [0.0f, 1.0f]
  float ratio = 255.0f;
  const auto colorComponents = ColorComponents{
      .red = static_cast<float>(r) / ratio,
      .green = static_cast<float>(g) / ratio,
      .blue = static_cast<float>(b) / ratio,
      .alpha = static_cast<float>(a) / ratio,
  };

  return hostPlatformColorFromComponents(colorComponents);
}

inline float alphaFromHostPlatformColor(Color color) {
  float ratio = 255.0f;
  return static_cast<float>(color.m_color.A) / ratio;
}

inline float redFromHostPlatformColor(Color color) {
  float ratio = 255.0f;
  return static_cast<float>(color.m_color.R) / ratio;
}

inline float greenFromHostPlatformColor(Color color) {
  float ratio = 255.0f;
  return static_cast<float>(color.m_color.G) / ratio;
}

inline float blueFromHostPlatformColor(Color color) {
  float ratio = 255.0f;
  return static_cast<float>(color.m_color.B) / ratio;
}

inline ColorComponents colorComponentsFromHostPlatformColor(Color color) {
  float ratio = 255;
  const auto windowsColor = color.AsWindowsColor();
  return ColorComponents{
      (float)windowsColor.R / ratio,
      (float)windowsColor.G / ratio,
      (float)windowsColor.B / ratio,
      (float)windowsColor.A / ratio};
}

} // namespace facebook::react

namespace std {
template <>
struct hash<facebook::react::Color> {
  size_t operator()(const facebook::react::Color &color) const {
    size_t h = color.m_color.A + (color.m_color.B << 8) + (color.m_color.G << 16) + (color.m_color.R << 24);

    for (auto s : color.m_platformColor) {
      h ^= hash<decltype(s)>{}(s);
    }

    return h;
  }
};
} // namespace std
