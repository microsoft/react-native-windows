// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/ResourceBrushUtils.h>
#include <Utils/ValueUtils.h>
#include <unicode.h>

#include <UI.Text.h>
#include <stdint.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Foundation.h>

#include <Views/ShadowNodeBase.h>

namespace Microsoft::ReactNative {

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

inline xaml::Thickness GetThickness(double thicknesses[(int)ShadowEdges::CountEdges]) {
  const double defaultWidth = std::max<double>(0, thicknesses[(int)ShadowEdges::AllEdges]);
  double startWidth = DefaultOrOverride(thicknesses[(int)ShadowEdges::Left], thicknesses[(int)ShadowEdges::Start]);
  double endWidth = DefaultOrOverride(thicknesses[(int)ShadowEdges::Right], thicknesses[(int)ShadowEdges::End]);

  // Compute each edge.  Most specific setting wins, so fill from broad to
  // narrow: all, horiz/vert, start/end, left/right
  xaml::Thickness thickness = {defaultWidth, defaultWidth, defaultWidth, defaultWidth};

  if (thicknesses[(int)ShadowEdges::Horizontal] != c_UndefinedEdge)
    thickness.Left = thickness.Right = thicknesses[(int)ShadowEdges::Horizontal];
  if (thicknesses[(int)ShadowEdges::Vertical] != c_UndefinedEdge)
    thickness.Top = thickness.Bottom = thicknesses[(int)ShadowEdges::Vertical];

  if (startWidth != c_UndefinedEdge)
    thickness.Left = startWidth;
  if (endWidth != c_UndefinedEdge)
    thickness.Right = endWidth;
  if (thicknesses[(int)ShadowEdges::Top] != c_UndefinedEdge)
    thickness.Top = thicknesses[(int)ShadowEdges::Top];
  if (thicknesses[(int)ShadowEdges::Bottom] != c_UndefinedEdge)
    thickness.Bottom = thicknesses[(int)ShadowEdges::Bottom];

  return thickness;
}

inline xaml::CornerRadius GetCornerRadius(double cornerRadii[(int)ShadowCorners::CountCorners]) {
  xaml::CornerRadius cornerRadius;
  const double defaultRadius = std::max<double>(0, cornerRadii[(int)ShadowCorners::AllCorners]);
  double topStartRadius =
      DefaultOrOverride(cornerRadii[(int)ShadowCorners::TopLeft], cornerRadii[(int)ShadowCorners::TopStart]);
  double topEndRadius =
      DefaultOrOverride(cornerRadii[(int)ShadowCorners::TopRight], cornerRadii[(int)ShadowCorners::TopEnd]);
  double bottomStartRadius =
      DefaultOrOverride(cornerRadii[(int)ShadowCorners::BottomLeft], cornerRadii[(int)ShadowCorners::BottomStart]);
  double bottomEndRadius =
      DefaultOrOverride(cornerRadii[(int)ShadowCorners::BottomRight], cornerRadii[(int)ShadowCorners::BottomEnd]);

  cornerRadius.TopLeft = DefaultOrOverride(defaultRadius, topStartRadius);
  cornerRadius.TopRight = DefaultOrOverride(defaultRadius, topEndRadius);
  cornerRadius.BottomLeft = DefaultOrOverride(defaultRadius, bottomStartRadius);
  cornerRadius.BottomRight = DefaultOrOverride(defaultRadius, bottomEndRadius);

  return cornerRadius;
}

template <class T>
void UpdatePadding(ShadowNodeBase *node, const T &element, ShadowEdges edge, double margin) {
  node->m_padding[(int)edge] = margin;
  xaml::Thickness thickness = GetThickness(node->m_padding);
  element.Padding(thickness);
}

template <class T>
void SetBorderThickness(ShadowNodeBase *node, const T &element, ShadowEdges edge, double margin) {
  node->m_border[(int)edge] = margin;
  xaml::Thickness thickness = GetThickness(node->m_border);
  element.BorderThickness(thickness);
}

template <class T>
void SetBorderBrush(const T &element, const xaml::Media::Brush &brush) {
  element.BorderBrush(brush);
}

template <class T>
bool TryUpdateBackgroundBrush(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "backgroundColor") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      const auto brush = react::uwp::BrushFrom(propertyValue);
      element.Background(brush);
      react::uwp::UpdateControlBackgroundResourceBrushes(element, brush);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::BackgroundProperty());
      react::uwp::UpdateControlBackgroundResourceBrushes(element, nullptr);
    }

    return true;
  }

  return false;
}

inline void UpdateCornerRadiusValueOnNode(
    ShadowNodeBase *node,
    ShadowCorners corner,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
      propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
    node->m_cornerRadius[(int)corner] = propertyValue.AsDouble();
  else
    node->m_cornerRadius[(int)corner] = c_UndefinedEdge;
}

template <class T>
void UpdateCornerRadiusOnElement(ShadowNodeBase *node, const T &element) {
  xaml::CornerRadius cornerRadius = GetCornerRadius(node->m_cornerRadius);
  element.CornerRadius(cornerRadius);
}

template <class T>
bool TryUpdateForeground(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "color") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      const auto brush = react::uwp::BrushFrom(propertyValue);
      element.Foreground(brush);
      react::uwp::UpdateControlForegroundResourceBrushes(element, brush);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::ForegroundProperty());
      react::uwp::UpdateControlForegroundResourceBrushes(element, nullptr);
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
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  bool isBorderProperty = true;

  if (propertyName == "borderColor") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      const auto brush = react::uwp::BrushFrom(propertyValue);
      element.BorderBrush(brush);
      react::uwp::UpdateControlBorderResourceBrushes(element, brush);
    } else if (propertyValue.IsNull()) {
      // If there's still a border thickness, use the default border brush.
      if (element.BorderThickness() != xaml::ThicknessHelper::FromUniformLength(0.0)) {
        element.BorderBrush(react::uwp::DefaultBrushStore::Instance().GetDefaultBorderBrush());
      } else {
        element.ClearValue(T::BorderBrushProperty());
      }
      react::uwp::UpdateControlBorderResourceBrushes(element, nullptr);
    }
  } else {
    auto iter = edgeTypeMap.find(propertyName);
    if (iter != edgeTypeMap.end()) {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        SetBorderThickness(node, element, iter->second, propertyValue.AsDouble());
        if (propertyValue.AsDouble() != 0 && !element.BorderBrush()) {
          // Borders with no brush draw something other than transparent on other platforms.
          // To match, we'll use a default border brush if one isn't already set.
          // Note:  Keep this in sync with code in ViewPanel::FinalizeProperties().
          element.BorderBrush(react::uwp::DefaultBrushStore::Instance().GetDefaultBorderBrush());
        }
      } else if (propertyValue.IsNull()) {
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
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  bool isPaddingProperty = true;

  if (propertyName == "paddingLeft") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Left, propertyValue.AsDouble());
  } else if (propertyName == "paddingTop") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Top, propertyValue.AsDouble());
  } else if (propertyName == "paddingRight") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Right, propertyValue.AsDouble());
  } else if (propertyName == "paddingBottom") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Bottom, propertyValue.AsDouble());
  } else if (propertyName == "paddingStart") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Start, propertyValue.AsDouble());
  } else if (propertyName == "paddingEnd") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::End, propertyValue.AsDouble());
  } else if (propertyName == "paddingHorizontal") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Horizontal, propertyValue.AsDouble());
  } else if (propertyName == "paddingVertical") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::Vertical, propertyValue.AsDouble());
  } else if (propertyName == "padding") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      UpdatePadding(node, element, ShadowEdges::AllEdges, propertyValue.AsDouble());
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
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
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
bool TryUpdateFontProperties(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  bool isFontProperty = true;

  if (propertyName == "fontSize") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      element.FontSize(propertyValue.AsDouble());
    else if (propertyValue.IsNull())
      element.ClearValue(T::FontSizeProperty());
  } else if (propertyName == "fontFamily") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
      element.FontFamily(xaml::Media::FontFamily(Microsoft::Common::Unicode::Utf8ToUtf16(propertyValue.AsString())));
    else if (propertyValue.IsNull())
      element.ClearValue(T::FontFamilyProperty());
  } else if (propertyName == "fontWeight") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &value = propertyValue.AsString();
      winrt::Windows::UI::Text::FontWeight fontWeight;
      if (value == "normal")
        fontWeight = text::FontWeights::Normal();
      else if (value == "bold")
        fontWeight = text::FontWeights::Bold();
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
        fontWeight = text::FontWeights::Normal();

      element.FontWeight(fontWeight);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::FontWeightProperty());
    }
  } else if (propertyName == "fontStyle") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      element.FontStyle((propertyValue.AsString() == "italic") ? text::FontStyle::Italic : text::FontStyle::Normal);
    } else if (propertyValue.IsNull()) {
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
    element.TextAlignment(xaml::TextAlignment::Left);
  else if (value == "right")
    element.TextAlignment(xaml::TextAlignment::Right);
  else if (value == "center")
    element.TextAlignment(xaml::TextAlignment::Center);
  else if (value == "justify")
    element.TextAlignment(xaml::TextAlignment::Justify);
  else
    element.TextAlignment(xaml::TextAlignment::DetectFromContent);
}

template <class T>
bool TryUpdateTextAlignment(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "textAlign") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &value = propertyValue.AsString();
      SetTextAlignment(element, value);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::TextAlignmentProperty());
    }

    return true;
  }

  return false;
}

template <class T>
void SetTextTrimming(const T &element, const std::string &value) {
  if (value == "clip")
    element.TextTrimming(xaml::TextTrimming::Clip);
  else if (value == "head" || value == "middle" || value == "tail") {
    // "head" and "middle" not supported by UWP, but "tail"
    // behavior is the most similar
    element.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
  } else
    element.TextTrimming(xaml::TextTrimming::None);
}

template <class T>
bool TryUpdateTextTrimming(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "ellipsizeMode") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &value = propertyValue.AsString();
      SetTextTrimming(element, value);
    } else if (propertyValue.IsNull()) {
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
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "textDecorationLine") {
    // FUTURE: remove when SDK target minVer >= 10.0.15063.0
    static bool isTextDecorationsSupported =
#ifndef USE_WINUI3
        winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(
            XAML_NAMESPACE_STR L".Controls.TextBlock", L"TextDecorations");
#else
        true;
#endif
    if (!isTextDecorationsSupported)
      return true;

    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      using winrt::Windows::UI::Text::TextDecorations;

      const std::string &value = propertyValue.AsString();
      TextDecorations decorations = TextDecorations::None;
      if (value == "none") {
        decorations = TextDecorations::None;
      } else if (value == "underline") {
        decorations = TextDecorations::Underline;
      } else if (value == "line-through") {
        decorations = TextDecorations::Strikethrough;
      } else if (value == "underline line-through") {
        decorations = TextDecorations::Underline | TextDecorations::Strikethrough;
      }

      element.TextDecorations(decorations);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::TextDecorationsProperty());
    }

    return true;
  }

  return false;
}

template <class T>
void SetFlowDirection(const T &element, const std::string &value) {
  if (value == "rtl")
    element.FlowDirection(xaml::FlowDirection::RightToLeft);
  else if (value == "ltr")
    element.FlowDirection(xaml::FlowDirection::LeftToRight);
  else // 'auto', 'inherit'
    element.ClearValue(xaml::FrameworkElement::FlowDirectionProperty());
}

template <class T>
bool TryUpdateFlowDirection(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if ((propertyName == "writingDirection") || (propertyName == "direction")) {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &value = propertyValue.AsString();
      SetFlowDirection(element, value);
    } else if (propertyValue.IsNull()) {
      element.ClearValue(T::FlowDirectionProperty());
    }

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateCharacterSpacing(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "letterSpacing" || propertyName == "characterSpacing") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      element.CharacterSpacing(propertyValue.AsInt32());
    else if (propertyValue.IsNull())
      element.ClearValue(T::CharacterSpacingProperty());

    return true;
  }

  return false;
}

template <class T>
bool TryUpdateOrientation(
    const T &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "orientation") {
    if (propertyValue.IsNull()) {
      element.ClearValue(T::OrientationProperty());
    } else if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &valueString = propertyValue.AsString();
      if (valueString == "horizontal")
        element.Orientation(xaml::Controls::Orientation::Horizontal);
      else if (valueString == "vertical")
        element.Orientation(xaml::Controls::Orientation::Vertical);
    }

    return true;
  }

  return false;
}

inline bool TryUpdateMouseEvents(
    ShadowNodeBase *node,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "onMouseEnter")
    node->m_onMouseEnterRegistered = !propertyValue.IsNull() && propertyValue.AsBoolean();
  else if (propertyName == "onMouseLeave")
    node->m_onMouseLeaveRegistered = !propertyValue.IsNull() && propertyValue.AsBoolean();
  else
    return false;

  return true;
}

} // namespace Microsoft::ReactNative
