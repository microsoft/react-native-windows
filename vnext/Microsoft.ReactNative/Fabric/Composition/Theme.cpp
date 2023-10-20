
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Theme.h"

#include <Fabric/Composition/CompositionUIService.h>
#include <winrt/Windows.UI.ViewManagement.h>

#ifndef CORE_ABI
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
#include <XamlUtils.h>

#endif // CORE_ABI

namespace Microsoft::ReactNative::Composition {

Theme::Theme(const winrt::Microsoft::ReactNative::ReactContext &reactContext) noexcept {
  m_compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          reactContext.Properties().Handle());
}

std::pair<bool, winrt::Windows::UI::Color> Theme::TryGetPlatformColor(const std::string &platformColor) noexcept {
  // Future: This could take into account the system theme

  if (auto cachedEntry = m_colorCache.find(platformColor); cachedEntry != m_colorCache.end())
    return cachedEntry->second;

#ifndef CORE_ABI
  // If XAML is loaded, look in application resources
  if (xaml::TryGetCurrentApplication()) {
    xaml::Media::Brush brush{Microsoft::ReactNative::BrushFromColorObject(platformColor)};
    if (auto scb{brush.try_as<xaml::Media::SolidColorBrush>()}) {
      auto pair = std::make_pair(true, scb.Color());
      m_colorCache[platformColor] = pair;
      return pair;
    }
  }
#endif // CORE_ABI

  // Accent colors
  // https://learn.microsoft.com/en-us/uwp/api/windows.ui.viewmanagement.uicolortype?view=winrt-22621
  static std::unordered_map<
      std::string,
      winrt::Windows::UI::ViewManagement::UIColorType,
      std::hash<std::string_view>,
      std::equal_to<>>
      s_uiColorTypes = {
          {"Accent", winrt::Windows::UI::ViewManagement::UIColorType::Accent},
          {"AccentDark1", winrt::Windows::UI::ViewManagement::UIColorType::AccentDark1},
          {"AccentDark2", winrt::Windows::UI::ViewManagement::UIColorType::AccentDark2},
          {"AccentDark3", winrt::Windows::UI::ViewManagement::UIColorType::AccentDark3},
          {"AccentLight1", winrt::Windows::UI::ViewManagement::UIColorType::AccentLight1},
          {"AccentLight2", winrt::Windows::UI::ViewManagement::UIColorType::AccentLight2},
          {"AccentLight3", winrt::Windows::UI::ViewManagement::UIColorType::AccentLight3},
          {"Background", winrt::Windows::UI::ViewManagement::UIColorType::Background},
          {"Complement", winrt::Windows::UI::ViewManagement::UIColorType::Complement},
          {"Foreground", winrt::Windows::UI::ViewManagement::UIColorType::Foreground}};

  auto uiColor = s_uiColorTypes.find(platformColor);
  if (uiColor != s_uiColorTypes.end()) {
    auto uiSettings{winrt::Windows::UI::ViewManagement::UISettings()};
    auto pair = std::make_pair(true, uiSettings.GetColorValue(uiColor->second));
    m_colorCache[platformColor] = pair;
    return pair;
  }

  // UI element colors
  // https://learn.microsoft.com/en-us/uwp/api/windows.ui.viewmanagement.uielementtype?view=winrt-22621
  static std::unordered_map<
      std::string,
      winrt::Windows::UI::ViewManagement::UIElementType,
      std::hash<std::string_view>,
      std::equal_to<>>
      s_uiElementTypes = {
          {"AccentColor", winrt::Windows::UI::ViewManagement::UIElementType::AccentColor},
          {"ActiveCaption", winrt::Windows::UI::ViewManagement::UIElementType::ActiveCaption},
          {"Background", winrt::Windows::UI::ViewManagement::UIElementType::Background},
          {"ButtonFace", winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace},
          {"ButtonText", winrt::Windows::UI::ViewManagement::UIElementType::ButtonText},
          {"CaptionText", winrt::Windows::UI::ViewManagement::UIElementType::CaptionText},
          {"GrayText", winrt::Windows::UI::ViewManagement::UIElementType::GrayText},
          {"Highlight", winrt::Windows::UI::ViewManagement::UIElementType::Highlight},
          {"HighlightText", winrt::Windows::UI::ViewManagement::UIElementType::HighlightText},
          {"Hotlight", winrt::Windows::UI::ViewManagement::UIElementType::Hotlight},
          {"InactiveCaption", winrt::Windows::UI::ViewManagement::UIElementType::InactiveCaption},
          {"InactiveCaptionText", winrt::Windows::UI::ViewManagement::UIElementType::InactiveCaptionText},
          {"NonTextHigh", winrt::Windows::UI::ViewManagement::UIElementType::NonTextHigh},
          {"NonTextLow", winrt::Windows::UI::ViewManagement::UIElementType::NonTextLow},
          {"NonTextMedium", winrt::Windows::UI::ViewManagement::UIElementType::NonTextMedium},
          {"NonTextMediumHigh", winrt::Windows::UI::ViewManagement::UIElementType::NonTextMediumHigh},
          {"NonTextMediumLow", winrt::Windows::UI::ViewManagement::UIElementType::NonTextMediumLow},
          {"OverlayOutsidePopup", winrt::Windows::UI::ViewManagement::UIElementType::OverlayOutsidePopup},
          {"PageBackground", winrt::Windows::UI::ViewManagement::UIElementType::PageBackground},
          {"PopupBackground", winrt::Windows::UI::ViewManagement::UIElementType::PopupBackground},
          {"TextContrastWithHigh", winrt::Windows::UI::ViewManagement::UIElementType::TextContrastWithHigh},
          {"TextHigh", winrt::Windows::UI::ViewManagement::UIElementType::TextHigh},
          {"TextLow", winrt::Windows::UI::ViewManagement::UIElementType::TextLow},
          {"TextMedium", winrt::Windows::UI::ViewManagement::UIElementType::TextMedium},
          {"Window", winrt::Windows::UI::ViewManagement::UIElementType::Window},
          {"WindowText", winrt::Windows::UI::ViewManagement::UIElementType::WindowText}};

  auto uiElement = s_uiElementTypes.find(platformColor);
  if (uiElement != s_uiElementTypes.end()) {
    auto uiSettings{winrt::Windows::UI::ViewManagement::UISettings()};
    auto pair = std::make_pair<bool, winrt::Windows::UI::Color>(true, uiSettings.UIElementColor(uiElement->second));
    m_colorCache[platformColor] = pair;
    return pair;
  }

  // Fallback light-theme XAML brushes
  // Issue #11489. These are all the light-theme values. Which is better than no values.
  // Where did these values come from? WinUI's common theme resources:
  // https://github.com/microsoft/microsoft-ui-xaml/blob/7a33ad772d77d908aa6b316ec24e6d2eb3ebf571/dev/CommonStyles/Common_themeresources_any.xaml
  // Specifically these are pulled from the "Light" ResourceDictionary. If any additional values
  // are needed, they should be taken from that section (not "Dark" or "HighContrast").
  // For control-specific values, they will be in a theme resource file for that control. Example:
  // https://github.com/microsoft/microsoft-ui-xaml/blob/9052972906c8a0a1b6cb5d5c61b27d6d27cd7f11/dev/CommonStyles/Button_themeresources.xaml

  static std::unordered_map<std::string, std::string, std::hash<std::string_view>, std::equal_to<>>
    s_xamlAliasedColors = {
       {"ButtonBackgroundPressed", "ControlFillColorTertiary"},
          {"ButtonForegroundPressed", "TextFillColorSecondary"},
          {"ButtonForegroundPointerOver", "TextFillColorPrimary"},
          {"ButtonBackground", "ControlFillColorDefault"},
          {"ButtonBorder", "ControlElevationBorder"},
          {"ControlElevationBorder", "ControlStrokeColorSecondary"}, // TODO ControlElevationBorderBrush is actually gradient brush
          {"ButtonForeground", "TextFillColorPrimary"},
          {"ButtonBackgroundDisabled", "ControlFillColorDisabled"},
          {"ButtonBorderDisabled", "ControlStrokeColorDefault"},
          {"ButtonForegroundDisabled", "TextFillColorDisabled"},
          {"ButtonBackgroundPointerOver", "ControlFillColorSecondary"},
          {"ButtonBorderPointerOver", "ControlElevationBorder"},
          {"ButtonBorderPressed", "ControlFillColorTransparent"},
          {"ToggleSwitchFillOff", "ControlAltFillColorSecondary"},
          {"ToggleSwitchFillOffPointerOver", "ControlAltFillColorTertiary"},
          {"ToggleSwitchFillOffPressed", "ControlAltFillColorQuarternary"},
          {"ToggleSwitchFillOffDisabled", "ControlAltFillColorDisabled"},
          {"ToggleSwitchStrokeOff", "ControlStrongStrokeColorDefault"},
          {"ToggleSwitchStrokeOffPointerOver", "ControlStrongStrokeColorDefault"},
          {"ToggleSwitchStrokeOffPressed", "ControlStrongStrokeColorDefault"},
          {"ToggleSwitchStrokeOffDisabled", "ControlStrongStrokeColorDisabled"},
          {"ToggleSwitchFillOn", "AccentFillColorDefault"},
          {"ToggleSwitchFillOnPointerOver", "AccentFillColorSecondary"},
          {"ToggleSwitchFillOnPressed", "AccentFillColorTertiary"},
          {"ToggleSwitchFillOnDisabled", "AccentFillColorDisabled"},
          {"ToggleSwitchStrokeOn", "AccentFillColorDefault"},
          {"ToggleSwitchStrokeOnPointerOver", "AccentFillColorSecondary"},
          {"ToggleSwitchStrokeOnPressed", "AccentFillColorTertiary"},
          {"ToggleSwitchStrokeOnDisabled", "AccentFillColorDisabled"},
          {"ToggleSwitchKnobFillOff", "TextFillColorSecondary"},
          {"ToggleSwitchKnobFillOffPointerOver", "TextFillColorSecondary"},
          {"ToggleSwitchKnobFillOffPressed", "TextFillColorSecondary"},
          {"ToggleSwitchKnobFillOffDisabled", "TextFillColorDisabled"},
          {"ToggleSwitchKnobFillOn", "TextOnAccentFillColorPrimary"},
          {"ToggleSwitchKnobFillOnPointerOver", "TextOnAccentFillColorPrimary"},
          {"ToggleSwitchKnobFillOnPressed", "TextOnAccentFillColorPrimary"},
          {"ToggleSwitchKnobFillOnDisabled", "TextOnAccentFillColorDisabled"},
          {"ToggleSwitchKnobStrokeOn", "CircleElevationBorder"},
          {"AccentFillColorDefault", "AccentDark1"}, // SystemAccentColorDark1
          {"AccentFillColorSecondary", "AccentDark1"}, // TODO SystemAccentColorDark1 + Opacity 0.9
          {"AccentFillColorTertiary", "AccentDark1"}, // TODO SystemAccentColorDark1 + Opacity 0.8
          {"CircleElevationBorder", "ControlStrokeColorDefault"} // TODO is actually a linear brush
    };

  static std::unordered_map<std::string, winrt::Windows::UI::Color, std::hash<std::string_view>, std::equal_to<>>
      s_xamlBrushes = {
          {"SolidBackgroundFillColorBase", {0xFF, 0xF3, 0xF3, 0xF3}},
          {"ControlFillColorDefault", {0xB3, 0xFF, 0xFF, 0xFF}},
          {"ControlFillColorSecondary", {0x80, 0xF9, 0xF9, 0xF9}},
          {"ControlFillColorTertiary", {0x4D, 0xF9, 0xF9, 0xF9}},
          {"ControlFillColorDisabled", {0x4D, 0xF9, 0xF9, 0xF9}},
          {"ControlFillColorTransparent", {0x00, 0xFF, 0xFF, 0xFF}},
          {"ControlStrokeColorDefault", {0x0F, 0x00, 0x00, 0x00}},
          {"ControlStrokeColorSecondary", {0x29, 0x00, 0x00, 0x00}},
          {"ControlStrokeColorOnAccentSecondary", {0x66, 0x00, 0x00, 0x00}},
          {"TextFillColorPrimary", {0xE4, 0x00, 0x00, 0x00}},
          {"TextFillColorSecondary", {0x9E, 0x00, 0x00, 0x00}},
          {"TextFillColorDisabled", {0x5C, 0x00, 0x00, 0x00}},
          {"ControlAltFillColorTertiary", {0x0F, 0x00, 0x00, 0x00}},
          {"ControlAltFillColorQuarternary", {0x18, 0x00, 0x00, 0x00}},
          {"ControlAltFillColorDisabled", {0x00, 0xFF, 0xFF, 0xFF}},
          {"ControlStrongStrokeColorDefault", {0x72, 0x00, 0x00, 0x00}},
          {"ControlStrongStrokeColorDisabled", {0x37, 0x00, 0x00, 0x00}},
          {"AccentFillColorDisabled", {0x37, 0x00, 0x00, 0x00}},
          {"TextOnAccentFillColorPrimary", {0xFF, 0xFF, 0xFF, 0xFF}},
          {"TextOnAccentFillColorDisabled", {0xFF, 0xFF, 0xFF, 0xFF}},
          {"ControlAltFillColorSecondary", {0x06, 0x00, 0x00, 0x00}},
          // Arguably only the control-agnostic platform colors should be respected, and
          // Button should be updated to use those instead, assuming that still holds up
          // in high contrast and such.
      };

  auto result = s_xamlBrushes.find(platformColor);
  if (result != s_xamlBrushes.end()) {
    auto pair = std::make_pair<bool, winrt::Windows::UI::Color>(true, winrt::Windows::UI::Color(result->second));
    m_colorCache[platformColor] = pair;
    return pair;
  }

  auto pair = std::make_pair<bool, winrt::Windows::UI::Color>(false, {});
  m_colorCache[platformColor] = pair;
  return pair;
}

winrt::Windows::UI::Color Theme::Color(const facebook::react::Color &color) noexcept {
  if (!color.m_platformColor.empty()) {
    for (auto platformColor : color.m_platformColor) {
      auto pair = TryGetPlatformColor(platformColor);
      if (pair.first) {
        return pair.second;
      }
    }
  }
  return color.m_color;
}

winrt::Windows::UI::Color Theme::PlatformColor(const std::string &platformColor) noexcept {
  auto pair = TryGetPlatformColor(platformColor);
  assert(pair.first);
  if (pair.first) {
    return pair.second;
  }

  return {0, 0, 0, 0}; // Transparent
}

winrt::Microsoft::ReactNative::Composition::IBrush Theme::PlatformBrush(const std::string &platformColor) noexcept {
  if (auto cachedEntry = m_platformColorBrushCache.find(platformColor); cachedEntry != m_platformColorBrushCache.end())
    return cachedEntry->second;

  auto pair = TryGetPlatformColor(platformColor);
  if (pair.first) {
    auto brush = m_compositionContext.CreateColorBrush(pair.second);
    m_platformColorBrushCache[platformColor] = brush;
    return brush;
  }
  return nullptr;
}

winrt::Microsoft::ReactNative::Composition::IBrush Theme::Brush(const facebook::react::Color &color) noexcept {
  if (!color.m_platformColor.empty()) {
    for (auto platformColor : color.m_platformColor) {
      auto brush = PlatformBrush(platformColor);
      if (brush)
        return brush;
    }
  }

  auto h = RGB(color.m_color.R, color.m_color.G, color.m_color.B) | (color.m_color.A << 24);
  if (auto cachedEntry = m_colorBrushCache.find(h); cachedEntry != m_colorBrushCache.end()) {
    return cachedEntry->second;
  }

  auto brush = m_compositionContext.CreateColorBrush(color.m_color);
  m_colorBrushCache[h] = brush;
  return brush;
}

D2D1::ColorF Theme::D2DColor(const facebook::react::Color& color) noexcept {
  auto c = Color(color);
  return {c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f};
}

D2D1::ColorF Theme::D2DPlatformColor(const std::string& platformColor) noexcept {
  auto c = PlatformColor(platformColor);
  return {c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f};
}

static const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<Theme>>>
    &ThemePropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<Theme>>>
      prop{L"ReactNative.Composition", L"Theme"};
  return prop;
}

std::shared_ptr<Theme> Theme::FromContext(const winrt::Microsoft::ReactNative::ReactContext &context) noexcept {
  return *context.Properties().GetOrCreate(ThemePropertyId(), [context]() { return std::make_shared<Theme>(context); });
}

} // namespace Microsoft::ReactNative::Composition
