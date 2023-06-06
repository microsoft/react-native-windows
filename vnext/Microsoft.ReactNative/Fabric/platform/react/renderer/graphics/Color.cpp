// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Color.h"
#include <Utils/ValueUtils.h>
#include <XamlUtils.h>

namespace facebook::react {

bool isColorMeaningful(SharedColor const &color) noexcept {
  if (!color) {
    return false;
  }

  return colorComponentsFromColor(color).alpha > 0;
}

winrt::Windows::UI::Color ResolvePlatformColor(Color const *const color) {
  if (!color->m_platformColor.empty()) {

#ifndef CORE_ABI
    // If XAML is loaded, look in application resources
    if (xaml::TryGetCurrentApplication()) {
      xaml::Media::Brush brush{Microsoft::ReactNative::BrushFromColorObject(winrt::to_hstring(color->m_platformColor))};
      if (auto scb{brush.try_as<xaml::Media::SolidColorBrush>()}) {
        return scb.Color();
      }
    }
#endif // CORE_ABI

    // Accent colors
    // https://learn.microsoft.com/en-us/uwp/api/windows.ui.viewmanagement.uicolortype?view=winrt-22621
    static std::unordered_map<std::string, ui::ViewManagement::UIColorType, std::hash<std::string_view>, std::equal_to<>>
            s_uiColorTypes = {
                {"Accent", ui::ViewManagement::UIColorType::Accent},
                {"AccentDark1", ui::ViewManagement::UIColorType::AccentDark1},
                {"AccentDark2", ui::ViewManagement::UIColorType::AccentDark2},
                {"AccentDark3", ui::ViewManagement::UIColorType::AccentDark3},
                {"AccentLight1", ui::ViewManagement::UIColorType::AccentLight1},
                {"AccentLight2", ui::ViewManagement::UIColorType::AccentLight2},
                {"AccentLight3", ui::ViewManagement::UIColorType::AccentLight3},
                {"Background", ui::ViewManagement::UIColorType::Background},
                {"Complement", ui::ViewManagement::UIColorType::Complement},
                {"Foreground", ui::ViewManagement::UIColorType::Foreground}};

    auto uiColor = s_uiColorTypes.find(color->m_platformColor);
    if (uiColor != s_uiColorTypes.end()) {
      auto uiSettings{ui::ViewManagement::UISettings()};
      return uiSettings.GetColorValue(uiColor->second);
    }

    // UI element colors
    // https://learn.microsoft.com/en-us/uwp/api/windows.ui.viewmanagement.uielementtype?view=winrt-22621
    static std::unordered_map<std::string, ui::ViewManagement::UIElementType, std::hash<std::string_view>, std::equal_to<>>
            s_uiElementTypes = {
                {"AccentColor", ui::ViewManagement::UIElementType::AccentColor},
                {"ActiveCaption", ui::ViewManagement::UIElementType::ActiveCaption},
                {"Background", ui::ViewManagement::UIElementType::Background},
                {"ButtonFace", ui::ViewManagement::UIElementType::ButtonFace},
                {"ButtonText", ui::ViewManagement::UIElementType::ButtonText},
                {"CaptionText", ui::ViewManagement::UIElementType::CaptionText},
                {"GrayText", ui::ViewManagement::UIElementType::GrayText},
                {"Highlight", ui::ViewManagement::UIElementType::Highlight},
                {"HighlightText", ui::ViewManagement::UIElementType::HighlightText},
                {"Hotlight", ui::ViewManagement::UIElementType::Hotlight},
                {"InactiveCaption", ui::ViewManagement::UIElementType::InactiveCaption},
                {"InactiveCaptionText", ui::ViewManagement::UIElementType::InactiveCaptionText},
                {"NonTextHigh", ui::ViewManagement::UIElementType::NonTextHigh},
                {"NonTextLow", ui::ViewManagement::UIElementType::NonTextLow},
                {"NonTextMedium", ui::ViewManagement::UIElementType::NonTextMedium},
                {"NonTextMediumHigh", ui::ViewManagement::UIElementType::NonTextMediumHigh},
                {"NonTextMediumLow", ui::ViewManagement::UIElementType::NonTextMediumLow},
                {"OverlayOutsidePopup", ui::ViewManagement::UIElementType::OverlayOutsidePopup},
                {"PageBackground", ui::ViewManagement::UIElementType::PageBackground},
                {"PopupBackground", ui::ViewManagement::UIElementType::PopupBackground},
                {"TextContrastWithHigh", ui::ViewManagement::UIElementType::TextContrastWithHigh},
                {"TextHigh", ui::ViewManagement::UIElementType::TextHigh},
                {"TextLow", ui::ViewManagement::UIElementType::TextLow},
                {"TextMedium", ui::ViewManagement::UIElementType::TextMedium},
                {"Window", ui::ViewManagement::UIElementType::Window},
                {"WindowText", ui::ViewManagement::UIElementType::WindowText}};

    auto uiElement = s_uiElementTypes.find(color->m_platformColor);
    if (uiElement != s_uiElementTypes.end()) {
      auto uiSettings{ui::ViewManagement::UISettings()};
      return uiSettings.UIElementColor(uiElement->second);
    }

    // Fallback light-theme XAML brushes
    // Issue #11489. These are all the light-theme values. Which is better than no values.
    // Where did these values come from? WinUI's common theme resources:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/7a33ad772d77d908aa6b316ec24e6d2eb3ebf571/dev/CommonStyles/Common_themeresources_any.xaml
    // Specifically these are pulled from the "Light" ResourceDictionary. If any additional values
    // are needed, they should be taken from that section (not "Dark" or "HighContrast").
    // For control-specific values, they will be in a theme resource file for that control. Example:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/9052972906c8a0a1b6cb5d5c61b27d6d27cd7f11/dev/CommonStyles/Button_themeresources.xaml
    static std::unordered_map<std::string, ui::Color, std::hash<std::string_view>, std::equal_to<>>
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
            // Arguably only the control-agnostic platform colors should be respected, and
            // Button should be updated to use those instead, assuming that still holds up
            // in high contrast and such.
            {"ButtonBackgroundPressed", {0x4D, 0xF9, 0xF9, 0xF9}}, // ControlFillColorTertiary
            {"ButtonForegroundPressed", {0x9E, 0x00, 0x00, 0x00}}, // TextFillColorSecondary
            {"ButtonForegroundPointerOver", {0xE4, 0x00, 0x00, 0x00}}, // TextFillColorPrimary
            {"ButtonBackground", {0xB3, 0xFF, 0xFF, 0xFF}}, // ControlFillColorDefault
            {"ButtonBorderBrush",
             {0x29, 0x00, 0x00, 0x00}}, // from ControlStrokeColorSecondary to ControlStrokeColorDefault
            {"ButtonForeground", {0xE4, 0x00, 0x00, 0x00}}, // TextFillColorPrimary
            {"ButtonBackgroundDisabled", {0x4D, 0xF9, 0xF9, 0xF9}}, // ControlFillColorDisabled
            {"ButtonBorderBrushDisabled", {0x0F, 0x00, 0x00, 0x00}}, // ControlStrokeColorDefault
            {"ButtonForegroundDisabled", {0x5C, 0x00, 0x00, 0x00}}, // TextFillColorDisabled
            {"ButtonBackgroundPointerOver", {0x80, 0xF9, 0xF9, 0xF9}}, // ControlFillColorSecondary
            {"ButtonBorderBrushPointerOver",
             {0x66, 0x00, 0x00, 0x00}}, // from ControlStrokeColorOnAccentSecondary to ControlStrokeColorOnAccentDefault
            {"ButtonBorderBrushPressed", {0x00, 0xFF, 0xFF, 0xFF}}, // ControlFillColorTransparent
        };

    auto result = s_xamlBrushes.find(color->m_platformColor);
    if (result != s_xamlBrushes.end()) {
      return result->second;
    }
  }

  return color->m_color;
}

D2D1::ColorF SharedColor::AsD2DColor() const {
  winrt::Windows::UI::Color color = ResolvePlatformColor(m_color.get());
  return {color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f};
}

winrt::Windows::UI::Color SharedColor::AsWindowsColor() const {
  return ResolvePlatformColor(m_color.get());
}

#ifndef CORE_ABI
xaml::Media::Brush SharedColor::AsWindowsBrush() const {
  if (!m_color)
    return nullptr;
  if (!m_color->m_platformColor.empty()) {
    return Microsoft::ReactNative::BrushFromColorObject(winrt::to_hstring(m_color->m_platformColor));
  }
  return xaml::Media::SolidColorBrush(m_color->m_color);
}
#endif // CORE_ABI

SharedColor colorFromComponents(ColorComponents components) {
  float ratio = 255;
  return {ui::ColorHelper::FromArgb(
      (int)round(components.alpha * ratio) & 0xff,
      (int)round(components.red * ratio) & 0xff,
      (int)round(components.green * ratio) & 0xff,
      (int)round(components.blue * ratio) & 0xff)};
}

ColorComponents colorComponentsFromColor(SharedColor const &sharedColor) {
  float ratio = 255;
  auto color = sharedColor.AsWindowsColor();
  return ColorComponents{
      (float)color.R / ratio, (float)color.G / ratio, (float)color.B / ratio, (float)color.A / ratio};
}

SharedColor clearColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 0});
  return color;
}

SharedColor blackColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 1});
  return color;
}

SharedColor whiteColor() {
  static SharedColor color = colorFromComponents(ColorComponents{1, 1, 1, 1});
  return color;
}

SharedColor greyColor() {
  static SharedColor color = colorFromComponents(ColorComponents{133, 133, 133, 1});
  return color;
}

} // namespace facebook::react
