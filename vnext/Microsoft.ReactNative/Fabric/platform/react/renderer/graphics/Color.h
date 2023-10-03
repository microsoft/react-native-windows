/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <limits>

#include <CppWinRTIncludes.h>
#include <d2d1_1.h>
#include <react/renderer/graphics/ColorComponents.h>
#include <optional>

namespace facebook {
namespace react {

struct Color {
  bool operator==(const Color &otherColor) const {
    return m_color == otherColor.m_color && m_platformColor == otherColor.m_platformColor;
  }
  bool operator!=(const Color &otherColor) const {
    return m_color != otherColor.m_color || m_platformColor != otherColor.m_platformColor;
  }

  winrt::Windows::UI::Color m_color;
  std::vector<std::string> m_platformColor;
};

/*
 * On Android, a color can be represented as 32 bits integer, so there is no
 * need to instantiate complex color objects and then pass them as shared
 * pointers. Hence instead of using shared_ptr, we use a simple wrapper class
 * which provides a pointer-like interface. On other platforms, colors may be
 * represented by more complex objects that cannot be represented as 32-bits
 * integers, so we hide the implementation detail in HostPlatformColor.h.
 */
class SharedColor {
  friend std::hash<SharedColor>;

 public:
  SharedColor() : m_color(nullptr) {}

  SharedColor(const SharedColor &sharedColor) : m_color(sharedColor.m_color) {}

  SharedColor(winrt::Windows::UI::Color color) {
    m_color = std::make_shared<Color>();
    m_color->m_color = color;
  }

  SharedColor(std::vector<std::string> &&windowsBrush) {
    m_color = std::make_shared<Color>();
    m_color->m_platformColor = std::move(windowsBrush);
  }

  SharedColor &operator=(const SharedColor &sharedColor) {
    m_color = sharedColor.m_color;
    return *this;
  }

  Color operator*() const {
    return *m_color;
  }

  bool operator==(const SharedColor &otherColor) const {
    if (!m_color && !otherColor.m_color)
      return true;
    if (!m_color || !otherColor.m_color)
      return false;
    return *m_color == *otherColor.m_color;
  }

  bool operator!=(const SharedColor &otherColor) const {
    if (!m_color && !otherColor.m_color)
      return false;
    if (!m_color || !otherColor.m_color)
      return true;
    return *m_color != *otherColor.m_color;
  }

  operator bool() const {
    return m_color != nullptr;
  }

  D2D1::ColorF AsD2DColor() const;

  winrt::Windows::UI::Color AsWindowsColor() const;

  COLORREF AsColorRefNoAlpha() const {
    return RGB(m_color->m_color.R, m_color->m_color.G, m_color->m_color.B);
  }

  COLORREF AsColorRefWithAlpha() const {
    return RGB(m_color->m_color.R, m_color->m_color.G, m_color->m_color.B) | (m_color->m_color.A << 24);
  }

#ifndef CORE_ABI
  xaml::Media::Brush AsWindowsBrush() const;
#endif

 private:
  std::shared_ptr<Color> m_color;
};

bool isColorMeaningful(const SharedColor &color) noexcept;
SharedColor colorFromComponents(ColorComponents components);
ColorComponents colorComponentsFromColor(SharedColor const &color);

SharedColor clearColor();
SharedColor blackColor();
SharedColor whiteColor();
SharedColor greyColor();

} // namespace react
} // namespace facebook

namespace std {
template <>
struct hash<facebook::react::SharedColor> {
  size_t operator()(const facebook::react::SharedColor &sharedColor) const {
    size_t h = sharedColor.m_color->m_color.A + (sharedColor.m_color->m_color.B << 8) +
        (sharedColor.m_color->m_color.G << 16) + (sharedColor.m_color->m_color.R << 24);

    for (auto s : sharedColor.m_color->m_platformColor) {
      h ^= hash<decltype(s)>{}(s);
    }

    return h;
  }
};
} // namespace std
