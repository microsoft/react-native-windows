// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <CppWinRTIncludes.h>
#include <d2d1_1.h>
#include <react/renderer/graphics/ColorComponents.h>
#include "PlatformColorUtils.h"

namespace facebook::react {

struct Color {
  bool operator==(const Color &otherColor) const {
    return m_isUndefined && otherColor.m_isUndefined ||
        (m_isUndefined == otherColor.m_isUndefined && m_color == otherColor.m_color &&
         m_platformColor == otherColor.m_platformColor);
  }
  bool operator!=(const Color &otherColor) const {
    return m_isUndefined != otherColor.m_isUndefined || m_color != otherColor.m_color ||
        m_platformColor != otherColor.m_platformColor;
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

  bool m_isUndefined;
  winrt::Windows::UI::Color m_color;
  std::vector<std::string> m_platformColor;
};

namespace HostPlatformColor {
static const facebook::react::Color UndefinedColor{true};
} // namespace HostPlatformColor

inline Color hostPlatformColorFromComponents(ColorComponents components) {
  float ratio = 255;
  const winrt::Windows::UI::Color color = {
      static_cast<uint8_t>((int)round(components.alpha * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.red * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.green * ratio) & 0xff),
      static_cast<uint8_t>((int)round(components.blue * ratio) & 0xff)};
  return {
      /* .m_isUndefined = */ false,
      /* .m_color = */ color,
      /* .m_platformColor = */ {}};
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
