// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include <array>
#include <bitset>
#include <cmath>
#include <optional>

namespace facebook {
namespace react {

struct WindowsViewEvents {
  std::bitset<32> bits{};

  enum class Offset : std::size_t {
    Focus = 0,
    Blur = 1,
    KeyUp = 2,
    KeyDown = 3,
    MouseEnter = 4,
    MouseLeave = 5,
  };

  constexpr bool operator[](const Offset offset) const {
    return bits[static_cast<std::size_t>(offset)];
  }

  std::bitset<32>::reference operator[](const Offset offset) {
    return bits[static_cast<std::size_t>(offset)];
  }
};

inline static bool operator==(WindowsViewEvents const &lhs, WindowsViewEvents const &rhs) {
  return lhs.bits == rhs.bits;
}

inline static bool operator!=(WindowsViewEvents const &lhs, WindowsViewEvents const &rhs) {
  return lhs.bits != rhs.bits;
}

static inline WindowsViewEvents convertRawProp(
    const PropsParserContext &context,
    const RawProps &rawProps,
    const WindowsViewEvents &sourceValue,
    const WindowsViewEvents &defaultValue) {
  WindowsViewEvents result{};
  using Offset = WindowsViewEvents::Offset;

  // Focus Events
  result[Offset::Focus] =
      convertRawProp(context, rawProps, "onFocus", sourceValue[Offset::Focus], defaultValue[Offset::Focus]);
  result[Offset::Blur] =
      convertRawProp(context, rawProps, "onBlur", sourceValue[Offset::Blur], defaultValue[Offset::Blur]);

  // Key Events
  result[Offset::KeyDown] =
      convertRawProp(context, rawProps, "onKeyDown", sourceValue[Offset::KeyDown], defaultValue[Offset::KeyDown]);
  result[Offset::KeyUp] =
      convertRawProp(context, rawProps, "onKeyUp", sourceValue[Offset::KeyUp], defaultValue[Offset::KeyUp]);

  // Mouse Events
  result[Offset::MouseEnter] = convertRawProp(
      context, rawProps, "onMouseEnter", sourceValue[Offset::MouseEnter], defaultValue[Offset::MouseEnter]);
  result[Offset::MouseLeave] = convertRawProp(
      context, rawProps, "onMouseLeave", sourceValue[Offset::MouseLeave], defaultValue[Offset::MouseLeave]);

  return result;
}

} // namespace react
} // namespace facebook
