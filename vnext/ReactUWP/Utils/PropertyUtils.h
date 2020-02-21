// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Utils/ResourceBrushUtils.h>
#include <Utils/ValueUtils.h>

#include <folly/dynamic.h>
#include <stdint.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <Views/ShadowNodeBase.h>

#include <Views/ViewPanel.h>

namespace winrt {
using namespace Windows::UI::Xaml;
}

namespace react {
namespace uwp {

struct ShadowNodeBase;

static double DefaultOrOverride(double defaultValue, double x) {
  return x != c_UndefinedEdge ? x : defaultValue;
};

static const std::unordered_map<std::string, ShadowEdges> edgeTypeMap = {
    {"borderLeftWidth", ShadowEdges::Left},
    {"borderTopWidth", ShadowEdges::Top},
    {"borderRightWidth", ShadowEdges::Right},
    {"borderBottomWidth", ShadowEdges::Bottom},
    {"borderStartWidth", ShadowEdges::Start},
    {"borderEndWidth", ShadowEdges::End},
    {"borderWidth", ShadowEdges::AllEdges},
};

inline winrt::Windows::UI::Xaml::Thickness GetThickness(double thicknesses[ShadowEdges::CountEdges], bool isRTL) {
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

inline winrt::Windows::UI::Xaml::CornerRadius GetCornerRadius(
    double cornerRadii[ShadowCorners::CountCorners],
    bool isRTL) {
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

template <class T>
void UpdatePadding(ShadowNodeBase *node, const T &element, ShadowEdges edge, double margin) {
  node->m_padding[edge] = margin;
  winrt::Thickness thickness =
      GetThickness(node->m_padding, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.Padding(thickness);
}

template <class T>
void SetBorderThickness(ShadowNodeBase *node, const T &element, ShadowEdges edge, double margin) {
  node->m_border[edge] = margin;
  winrt::Thickness thickness =
      GetThickness(node->m_border, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.BorderThickness(thickness);
}

template <class T>
void SetBorderBrush(const T &element, const winrt::Windows::UI::Xaml::Media::Brush &brush) {
  element.BorderBrush(brush);
}

template <class T>
bool TryUpdateBackgroundBrush(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "backgroundColor") {
    if (IsValidColorValue(propertyValue)) {
      const auto brush = BrushFrom(propertyValue);
      element.Background(brush);
      UpdateControlBackgroundResourceBrushes(element, brush);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::BackgroundProperty());
      UpdateControlBackgroundResourceBrushes(element, nullptr);
    }

    return true;
  }

  return false;
}

inline void
UpdateCornerRadiusValueOnNode(ShadowNodeBase *node, ShadowCorners corner, const folly::dynamic &propertyValue) {
  if (propertyValue.isNumber())
    node->m_cornerRadius[corner] = propertyValue.asDouble();
  else
    node->m_cornerRadius[corner] = c_UndefinedEdge;
}

template <class T>
void UpdateCornerRadiusOnElement(ShadowNodeBase *node, const T &element) {
  winrt::CornerRadius cornerRadius =
      GetCornerRadius(node->m_cornerRadius, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.CornerRadius(cornerRadius);
}

template <class T>
bool TryUpdateForeground(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "color") {
    if (IsValidColorValue(propertyValue)) {
      const auto brush = BrushFrom(propertyValue);
      element.Foreground(brush);
      UpdateControlForegroundResourceBrushes(element, brush);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::ForegroundProperty());
      UpdateControlForegroundResourceBrushes(element, nullptr);
    }

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateBorderProperties(
    ShadowNodeBase *node,
    const T &element,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  bool isBorderProperty = true;

  if (propertyName == "borderColor") {
    if (IsValidColorValue(propertyValue)) {
      const auto brush = BrushFrom(propertyValue);
      element.BorderBrush(brush);
      UpdateControlBorderResourceBrushes(element, brush);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::BorderBrushProperty());
      UpdateControlBorderResourceBrushes(element, nullptr);
    }
  } else {
    auto iter = edgeTypeMap.find(propertyName);
    if (iter != edgeTypeMap.end()) {
      if (propertyValue.isNumber()) {
        SetBorderThickness(node, element, iter->second, propertyValue.asDouble());
      } else if (propertyValue.isNull()) {
        SetBorderThickness(node, element, iter->second, 0);
      }
    } else {
      isBorderProperty = false;
    }
  }

  return isBorderProperty;
}

template <class T>
bool TryUpdatePadding(
    ShadowNodeBase *node,
    const T &element,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Left, propertyValue.asDouble());
  } else if (propertyName == "paddingTop") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Top, propertyValue.asDouble());
  } else if (propertyName == "paddingRight") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Right, propertyValue.asDouble());
  } else if (propertyName == "paddingBottom") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Bottom, propertyValue.asDouble());
  } else if (propertyName == "paddingStart") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Start, propertyValue.asDouble());
  } else if (propertyName == "paddingEnd") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::End, propertyValue.asDouble());
  } else if (propertyName == "paddingHorizontal") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Horizontal, propertyValue.asDouble());
  } else if (propertyName == "paddingVertical") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::Vertical, propertyValue.asDouble());
  } else if (propertyName == "padding") {
    if (propertyValue.isNumber())
      UpdatePadding(node, element, ShadowEdges::AllEdges, propertyValue.asDouble());
  } else {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

template <class T>
bool TryUpdateCornerRadiusOnNode(
    ShadowNodeBase *node,
    const T & /*element*/,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  if (propertyName == "borderTopLeftRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::TopLeft, propertyValue);
  } else if (propertyName == "borderTopRightRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::TopRight, propertyValue);
  } else if (propertyName == "borderTopStartRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::TopStart, propertyValue);
  } else if (propertyName == "borderTopEndRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::TopEnd, propertyValue);
  } else if (propertyName == "borderBottomRightRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::BottomRight, propertyValue);
  } else if (propertyName == "borderBottomLeftRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::BottomLeft, propertyValue);
  } else if (propertyName == "borderBottomStartRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::BottomStart, propertyValue);
  } else if (propertyName == "borderBottomEndRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::BottomEnd, propertyValue);
  } else if (propertyName == "borderRadius") {
    UpdateCornerRadiusValueOnNode(node, ShadowCorners::AllCorners, propertyValue);
  } else {
    return false;
  }

  return true;
}

template <class T>
winrt::Windows::UI::Text::FontWeight SetFontWeight(const T &element, const folly::dynamic &propertyValue) {
  winrt::Windows::UI::Text::FontWeight fontWeight;

  if (propertyValue.isString()) {
    const std::string &value = propertyValue.getString();

    if (value == "normal")
      fontWeight = winrt::Windows::UI::Text::FontWeights::Normal();
    else if (value == "bold")
      fontWeight = winrt::Windows::UI::Text::FontWeights::Bold();
    else if (value == "100")
      fontWeight.Weight = 100;
    else if (value == "200")
      fontWeight.Weight = 200;
    else if (value == "300")
      fontWeight.Weight = 300;
    else if (value == "400")
      fontWeight.Weight = 400;
    else if (value == "500")
      fontWeight.Weight = 500;
    else if (value == "600")
      fontWeight.Weight = 600;
    else if (value == "700")
      fontWeight.Weight = 700;
    else if (value == "800")
      fontWeight.Weight = 800;
    else if (value == "900")
      fontWeight.Weight = 900;
    else
      fontWeight = winrt::Windows::UI::Text::FontWeights::Normal();

    element.FontWeight(fontWeight);
  } else if (propertyValue.isNull()) {
    element.ClearValue(T::FontWeightProperty());
    fontWeight = element.FontWeight();
  } else {
    fontWeight = element.FontWeight();
  }

  return fontWeight;
}

template <class T>
winrt::Windows::UI::Text::FontStyle SetFontStyle(const T &element, const folly::dynamic &propertyValue) {
  winrt::Windows::UI::Text::FontStyle fontStyle;

  if (propertyValue.isString()) {
    if (propertyValue.getString() == "italic") {
      fontStyle = winrt::Windows::UI::Text::FontStyle::Italic;
    } else {
      fontStyle = winrt::Windows::UI::Text::FontStyle::Normal;
    }

    element.FontStyle(fontStyle);
  } else if (propertyValue.isNull()) {
    element.ClearValue(T::FontStyleProperty());
    fontStyle = element.FontStyle();
  } else {
    fontStyle = element.FontStyle();
  }

  return fontStyle;
}

// A "font family" (or "typeface") is composed of individual fonts. Each font is a set of glyphs with a specific weight
// and style. For example, "Arial" is a font family that contains the fonts "Arial", "Arial Italic", "Arial Bold", and
// "Arial Bold Italic". Each font is stored in a separate font file on disk.
//
// When using system fonts, FontFamily is set to the name of the typeface. Setting FontWeight or FontStyle is then
// sufficient to select the correct font file from the family.
//
// When using custom fonts, however, FontFamily is set to a specific font file path. This XAML limitation means a custom
// FontFamily can only have one font. Thus to support weights and styles for custom fonts, we need to do some juggling
// of FontFamily.
template <class T>
void UpdateFontProperties(const T &element, const folly::dynamic &reactDiffMap) {
  const folly::dynamic *fontFamilyProp = nullptr;
  const folly::dynamic *fontWeightProp = nullptr;
  const folly::dynamic *fontStyleProp = nullptr;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "fontSize") {
      if (propertyValue.isNumber())
        element.FontSize(propertyValue.asDouble());
      else if (propertyValue.isNull())
        element.ClearValue(T::FontSizeProperty());
    } else if (propertyName == "fontFamily") {
      fontFamilyProp = &propertyValue;
    } else if (propertyName == "fontWeight") {
      fontWeightProp = &propertyValue;
    } else if (propertyName == "fontStyle") {
      fontStyleProp = &propertyValue;
    }
  }

  // Any time the fontFamily, fontWeight, or fontStyle props change, we need to update FontFamily to match
  // the new values.
  if (nullptr != fontFamilyProp || nullptr != fontWeightProp || nullptr != fontStyleProp) {
    winrt::Windows::UI::Text::FontWeight fontWeight;
    winrt::Windows::UI::Text::FontStyle fontStyle;

    if (nullptr != fontWeightProp) {
      fontWeight = SetFontWeight(element, *fontWeightProp);
    } else {
      // FontWeight didn't change, but may have been set previously
      fontWeight = element.FontWeight();
    }

    if (nullptr != fontStyleProp) {
      fontStyle = SetFontStyle(element, *fontStyleProp);
    } else {
      // FontStyle didn't change, but may have been set previously
      fontStyle = element.FontStyle();
    }

    if (nullptr != fontFamilyProp) {
      if (fontFamilyProp->isString()) {
        std::wstring fontFamily = asWStr(*fontFamilyProp);
        if (fontFamily.size() > 0) {
          element.FontFamily(FontFamilyFrom(fontFamily, fontWeight, fontStyle));
        }
      } else if (fontFamilyProp->isNull()) {
        element.ClearValue(T::FontFamilyProperty());
      }
    } else {
      // FontFamily didn't change, but may have been set previously
      winrt::hstring fontFamily = element.FontFamily().Source();
      if (fontFamily.size() > 0) {
        std::wstring f{fontFamily};
        element.FontFamily(FontFamilyFrom(f, fontWeight, fontStyle));
      }
    }
  }
}

template <class T>
void SetTextAlignment(const T &element, const std::string &value) {
  if (value == "left")
    element.TextAlignment(winrt::TextAlignment::Left);
  else if (value == "right")
    element.TextAlignment(winrt::TextAlignment::Right);
  else if (value == "center")
    element.TextAlignment(winrt::TextAlignment::Center);
  else if (value == "justify")
    element.TextAlignment(winrt::TextAlignment::Justify);
  else
    element.TextAlignment(winrt::TextAlignment::DetectFromContent);
}

template <class T>
bool TryUpdateTextAlignment(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "textAlign") {
    if (propertyValue.isString()) {
      const std::string &value = propertyValue.getString();
      SetTextAlignment(element, value);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::TextAlignmentProperty());
    }

    return true;
  }

  return false;
}

template <class T>
void SetTextTrimming(const T &element, const std::string &value) {
  if (value == "clip")
    element.TextTrimming(winrt::TextTrimming::Clip);
  else if (value == "head" || value == "middle" || value == "tail") {
    // "head" and "middle" not supported by UWP, but "tail"
    // behavior is the most similar
    element.TextTrimming(winrt::TextTrimming::CharacterEllipsis);
  } else
    element.TextTrimming(winrt::TextTrimming::None);
}

template <class T>
bool TryUpdateTextTrimming(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "ellipsizeMode") {
    if (propertyValue.isString()) {
      const std::string &value = propertyValue.getString();
      SetTextTrimming(element, value);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::TextTrimmingProperty());
    }

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateTextDecorationLine(
    const T &element,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  if (propertyName == "textDecorationLine") {
    // FUTURE: remove when SDK target minVer >= 10.0.15063.0
    static bool isTextDecorationsSupported = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(
        L"Windows.UI.Xaml.Controls.TextBlock", L"TextDecorations");
    if (!isTextDecorationsSupported)
      return true;

    if (propertyValue.isString()) {
      using winrt::Windows::UI::Text::TextDecorations;

      const std::string &value = propertyValue.getString();
      TextDecorations decorations = TextDecorations::None;
      if (value == "none")
        decorations = TextDecorations::None;
      else if (value == "underline")
        decorations = TextDecorations::Underline;
      else if (value == "line-through")
        decorations = TextDecorations::Strikethrough;
      else if (value == "underline line-through")
        decorations = TextDecorations::Underline | TextDecorations::Strikethrough;

      element.TextDecorations(decorations);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::TextDecorationsProperty());
    }

    return true;
  }

  return false;
}

template <class T>
void SetFlowDirection(const T &element, const std::string &value) {
  if (value == "rtl")
    element.FlowDirection(winrt::FlowDirection::RightToLeft);
  else if (value == "ltr")
    element.FlowDirection(winrt::FlowDirection::LeftToRight);
  else // 'auto', 'inherit'
    element.ClearValue(winrt::FrameworkElement::FlowDirectionProperty());
}

template <class T>
bool TryUpdateFlowDirection(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if ((propertyName == "writingDirection") || (propertyName == "direction")) {
    if (propertyValue.isString()) {
      const std::string &value = propertyValue.getString();
      SetFlowDirection(element, value);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::FlowDirectionProperty());
    }

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateCharacterSpacing(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing") {
    if (propertyValue.isNumber())
      element.CharacterSpacing(static_cast<int32_t>(propertyValue.asDouble()));
    else if (propertyValue.isNull())
      element.ClearValue(T::CharacterSpacingProperty());

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateOrientation(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "orientation") {
    if (propertyValue.isNull()) {
      element.ClearValue(T::OrientationProperty());
    } else if (propertyValue.isString()) {
      const std::string &valueString = propertyValue.getString();
      if (valueString == "horizontal")
        element.Orientation(Orientation::Horizontal);
      else if (valueString == "vertical")
        element.Orientation(Orientation::Vertical);
    }

    return true;
  }

  return false;
}

inline bool
TryUpdateMouseEvents(ShadowNodeBase *node, const std::string &propertyName, const folly::dynamic &propertyValue) {
  if (propertyName == "onMouseEnter")
    node->m_onMouseEnter = !propertyValue.isNull() && propertyValue.asBool();
  else if (propertyName == "onMouseLeave")
    node->m_onMouseLeave = !propertyValue.isNull() && propertyValue.asBool();
  else if (propertyName == "onMouseMove")
    node->m_onMouseMove = !propertyValue.isNull() && propertyValue.asBool();
  else
    return false;

  return true;
}

} // namespace uwp
} // namespace react
