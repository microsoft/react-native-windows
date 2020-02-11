// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/PropertyUtils.h>

namespace react {
namespace uwp {

bool TryGetBorderEdgeType(const std::string &propertyName, ShadowEdges *result) {
  static const std::unordered_map<std::string, ShadowEdges> borderTypeMap = {
      {"borderLeftWidth", ShadowEdges::Left},
      {"borderTopWidth", ShadowEdges::Top},
      {"borderRightWidth", ShadowEdges::Right},
      {"borderBottomWidth", ShadowEdges::Bottom},
      {"borderStartWidth", ShadowEdges::Start},
      {"borderEndWidth", ShadowEdges::End},
      {"borderWidth", ShadowEdges::AllEdges},
  };

  auto iter = borderTypeMap.find(propertyName);
  if (iter != borderTypeMap.end()) {
    if (nullptr != result) {
      *result = iter->second;
    }
    return true;
  }
  return false;
}

bool TryGetBorderRadiusCornerType(const std::string &propertyName, ShadowCorners *result) {
  static const std::unordered_map<std::string, ShadowCorners> borderRadiusTypeMap = {
      {"borderTopLeftRadius", ShadowCorners::TopLeft},
      {"borderTopRightRadius", ShadowCorners::TopRight},
      {"borderBottomLeftRadius", ShadowCorners::BottomLeft},
      {"borderBottomRightRadius", ShadowCorners::BottomRight},
      {"borderTopStartRadius", ShadowCorners::TopStart},
      {"borderTopEndRadius", ShadowCorners::TopEnd},
      {"borderBottomStartRadius", ShadowCorners::BottomStart},
      {"borderBottomEndRadius", ShadowCorners::BottomEnd},
      {"borderRadius", ShadowCorners::AllCorners},
  };

  auto iter = borderRadiusTypeMap.find(propertyName);
  if (iter != borderRadiusTypeMap.end()) {
    if (nullptr != result) {
      *result = iter->second;
    }
    return true;
  }
  return false;
}

bool TryGetPaddingEdgeType(const std::string &propertyName, ShadowEdges *result) {
  static const std::unordered_map<std::string, ShadowEdges> paddingTypeMap = {
      {"paddingLeft", ShadowEdges::Left},
      {"paddingTop", ShadowEdges::Top},
      {"paddingRight", ShadowEdges::Right},
      {"paddingBottom", ShadowEdges::Bottom},
      {"paddingStart", ShadowEdges::Start},
      {"paddingEnd", ShadowEdges::End},
      {"paddingHorizontal", ShadowEdges::Horizontal},
      {"paddingVertical", ShadowEdges::Vertical},
      {"padding", ShadowEdges::AllEdges},
  };

  auto iter = paddingTypeMap.find(propertyName);
  if (iter != paddingTypeMap.end()) {
    if (nullptr != result) {
      *result = iter->second;
    }
    return true;
  }
  return false;
}

double DefaultOrOverride(double defaultValue, double x) {
  return x != c_UndefinedEdge ? x : defaultValue;
};

winrt::Windows::UI::Xaml::Thickness GetThickness(double thicknesses[ShadowEdges::CountEdges], bool isRTL) {
  const double defaultWidth = std::max<double>(0, thicknesses[ShadowEdges::AllEdges]);
  double startWidth = DefaultOrOverride(thicknesses[ShadowEdges::Left], thicknesses[ShadowEdges::Start]);
  double endWidth = DefaultOrOverride(thicknesses[ShadowEdges::Right], thicknesses[ShadowEdges::End]);
  if (isRTL)
    std::swap(startWidth, endWidth);

  // Compute each edge.  Most specific setting wins, so fill from broad to
  // narrow: all, horiz/vert, start/end, left/right
  winrt::Windows::UI::Xaml::Thickness thickness = {defaultWidth, defaultWidth, defaultWidth, defaultWidth};

  if (thicknesses[ShadowEdges::Horizontal] != c_UndefinedEdge)
    thickness.Left = thickness.Right = thicknesses[ShadowEdges::Horizontal];
  if (thicknesses[ShadowEdges::Vertical] != c_UndefinedEdge)
    thickness.Top = thickness.Bottom = thicknesses[ShadowEdges::Vertical];

  if (startWidth != c_UndefinedEdge)
    thickness.Left = startWidth;
  if (endWidth != c_UndefinedEdge)
    thickness.Right = endWidth;
  if (thicknesses[ShadowEdges::Top] != c_UndefinedEdge)
    thickness.Top = thicknesses[ShadowEdges::Top];
  if (thicknesses[ShadowEdges::Bottom] != c_UndefinedEdge)
    thickness.Bottom = thicknesses[ShadowEdges::Bottom];

  return thickness;
}

winrt::Windows::UI::Xaml::CornerRadius GetCornerRadius(double cornerRadii[ShadowCorners::CountCorners], bool isRTL) {
  winrt::Windows::UI::Xaml::CornerRadius cornerRadius;
  const double defaultRadius = std::max<double>(0, cornerRadii[ShadowCorners::AllCorners]);
  double topStartRadius = DefaultOrOverride(cornerRadii[ShadowCorners::TopLeft], cornerRadii[ShadowCorners::TopStart]);
  double topEndRadius = DefaultOrOverride(cornerRadii[ShadowCorners::TopRight], cornerRadii[ShadowCorners::TopEnd]);
  double bottomStartRadius =
      DefaultOrOverride(cornerRadii[ShadowCorners::BottomLeft], cornerRadii[ShadowCorners::BottomStart]);
  double bottomEndRadius =
      DefaultOrOverride(cornerRadii[ShadowCorners::BottomRight], cornerRadii[ShadowCorners::BottomEnd]);
  if (isRTL) {
    std::swap(topStartRadius, topEndRadius);
    std::swap(bottomStartRadius, bottomEndRadius);
  }

  cornerRadius.TopLeft = DefaultOrOverride(defaultRadius, topStartRadius);
  cornerRadius.TopRight = DefaultOrOverride(defaultRadius, topEndRadius);
  cornerRadius.BottomLeft = DefaultOrOverride(defaultRadius, bottomStartRadius);
  cornerRadius.BottomRight = DefaultOrOverride(defaultRadius, bottomEndRadius);

  return cornerRadius;
}

} // namespace uwp
} // namespace react
