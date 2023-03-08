// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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

} // namespace react
} // namespace facebook
