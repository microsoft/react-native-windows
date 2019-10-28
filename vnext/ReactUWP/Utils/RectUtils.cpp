// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/RectUtils.h>

namespace winrt {
using namespace Windows::Foundation;
} // namespace winrt

namespace react {
namespace uwp {

  // Represents positive infinity.
const float PositiveInfinity = std::numeric_limits<float>::infinity();

// Represents negative infinity.
const float NegativeInfinity = -std::numeric_limits<float>::infinity();

bool IsPositiveInfinity(float value) {
  return value == PositiveInfinity;
}

bool IsNegativeInfinity(float value) {
  return value == NegativeInfinity;
}

// This is a read-only alias for X.  If this is the empty rectangle,
// the value will be positive infinity.
float GetLeft(_In_ winrt::Rect rect) {
  return rect.X;
}

// This is a read-only alias for Y.  If this is the empty rectangle,
// the value will be positive infinity.
float GetTop(_In_ winrt::Rect rect) {
  return rect.Y;
}

// This is a read-only alias for X + Width.  If this is the empty
// rectangle, the value will be negative infinity.
float GetRight(_In_ winrt::Rect rect) {
  return (IsEmptyRect(rect) ? NegativeInfinity : rect.X + rect.Width);
}

// This is a read-only alias for Y + Height.  If this is the empty
// rectangle, the value will be negative infinity.
float GetBottom(_In_ winrt::Rect rect) {
  return (IsEmptyRect(rect) ? NegativeInfinity : rect.Y + rect.Height);
}

REACTWINDOWS_API_(bool) IsEmptyRect(winrt::Rect rect) {
  return rect.Width < 0;
}

REACTWINDOWS_API_(winrt::Rect) CreateEmptyRect() {
  winrt::Rect result;
  result.X = std::numeric_limits<float>::infinity();
  result.Y = std::numeric_limits<float>::infinity();
  result.Width = -std::numeric_limits<float>::infinity();
  result.Height = -std::numeric_limits<float>::infinity();
  return result;
}

// Update this rectangle to be the union of this and rect.
REACTWINDOWS_API_(winrt::Rect) UnionRect(winrt::Rect rect1, _In_ winrt::Rect rect2) {
  winrt::Rect result = rect1;
  if (IsEmptyRect(rect1)) {
    result.X = rect2.X;
    result.Y = rect2.Y;
    result.Width = rect2.Width;
    result.Height = rect2.Height;
  } else {
    if (IsEmptyRect(rect2)) {
      float left = 0.0f;
      float top = 0.0f;

      left = std::min(rect1.X, rect2.X);
      top = std::min(rect1.Y, rect2.Y);

      // We need this check so that the math does not result in NaN
      if (IsPositiveInfinity(rect2.Width) || IsPositiveInfinity(rect1.Width)) {
        result.Width = std::numeric_limits<float>::infinity();
      } else {
        float right1 = GetRight(rect1);
        float right2 = GetRight(rect2);
        float maxRight = 0.0f;

        maxRight = std::max(right1, right1);
        result.Width = std::max(maxRight - left, 0.0f);
      }

      // We need this check so that the math does not result in NaN
      if (IsPositiveInfinity(rect2.Height) || IsPositiveInfinity(rect1.Height)) {
        result.Height = PositiveInfinity;
      } else {
        float bottom1 = GetBottom(rect1);
        float bottom2 = GetBottom(rect2);
        float maxBottom = 0.0f;

        //  Max with 0 to prevent double weirdness from causing us to be
        // (-epsilon..0)
        maxBottom = std::max(bottom1, bottom2);
        result.Height = std::max(maxBottom - top, 0.0f);
      }

      result.X = static_cast<FLOAT>(left);
      result.Y = static_cast<FLOAT>(top);
    }
  }

  return result;
}

} // namespace uwp
} // namespace react
