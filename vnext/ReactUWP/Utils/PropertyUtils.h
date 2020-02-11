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

bool TryGetBorderEdgeType(const std::string &propertyName, ShadowEdges *result);
bool TryGetBorderRadiusCornerType(const std::string &propertyName, ShadowCorners *result);
bool TryGetPaddingEdgeType(const std::string &propertyName, ShadowEdges *result);

winrt::Windows::UI::Xaml::Thickness GetThickness(double thicknesses[ShadowEdges::CountEdges], bool isRTL);
winrt::Windows::UI::Xaml::CornerRadius GetCornerRadius(double cornerRadii[ShadowCorners::CountCorners], bool isRTL);

template <class T>
void UpdatePadding(ShadowNodeBase *node, const T &element, ShadowEdges edge, const folly::dynamic &propertyValue) {
  node->m_padding[edge] = propertyValue.isNumber() ? propertyValue.asDouble() : c_UndefinedEdge;
  winrt::Thickness thickness =
      GetThickness(node->m_padding, element.FlowDirection() == winrt::FlowDirection::RightToLeft);
  element.Padding(thickness);
}

template <class T>
void SetBorderThickness(ShadowNodeBase *node, const T &element, ShadowEdges edge, const folly::dynamic &propertyValue) {
  node->m_border[edge] = propertyValue.isNumber() ? propertyValue.asDouble() : c_UndefinedEdge;
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
    ShadowEdges edgeType;
    if (TryGetBorderEdgeType(propertyName, &edgeType)) {
      SetBorderThickness(node, element, edgeType, propertyValue);
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

  ShadowEdges edgeType;
  if (TryGetPaddingEdgeType(propertyName, &edgeType)) {
    UpdatePadding(node, element, edgeType, propertyValue);
  } else {
    isPaddingProperty = false;
  }

  return isPaddingProperty;
}

template <class T>
bool TryUpdateCornerRadiusOnNode(
    ShadowNodeBase *node,
    const T &element,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  bool isCornerRadiusProperty = true;

  ShadowCorners cornerType;
  if (TryGetBorderRadiusCornerType(propertyName, &cornerType)) {
    UpdateCornerRadiusValueOnNode(node, cornerType, propertyValue);
  } else {
    isCornerRadiusProperty = false;
  }

  return isCornerRadiusProperty;
}

template <class T>
bool TryUpdateFontProperties(const T &element, const std::string &propertyName, const folly::dynamic &propertyValue) {
  bool isFontProperty = true;

  if (propertyName == "fontSize") {
    if (propertyValue.isNumber())
      element.FontSize(propertyValue.asDouble());
    else if (propertyValue.isNull())
      element.ClearValue(T::FontSizeProperty());
  } else if (propertyName == "fontFamily") {
    if (propertyValue.isString())
      element.FontFamily(winrt::Windows::UI::Xaml::Media::FontFamily(asWStr(propertyValue)));
    else if (propertyValue.isNull())
      element.ClearValue(T::FontFamilyProperty());
  } else if (propertyName == "fontWeight") {
    if (propertyValue.isString()) {
      const std::string &value = propertyValue.getString();
      winrt::Windows::UI::Text::FontWeight fontWeight;
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
    }

  } else if (propertyName == "fontStyle") {
    if (propertyValue.isString()) {
      element.FontStyle(
          (propertyValue.getString() == "italic") ? winrt::Windows::UI::Text::FontStyle::Italic
                                                  : winrt::Windows::UI::Text::FontStyle::Normal);
    } else if (propertyValue.isNull()) {
      element.ClearValue(T::FontStyleProperty());
    }

  } else {
    isFontProperty = false;
  }

  return isFontProperty;
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
