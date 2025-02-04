// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "PlatformColorUtils.h"
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
#ifndef CORE_ABI
#include <XamlUtils.h>
#endif // CORE_ABI
#include <winrt/Windows.UI.ViewManagement.h>

namespace facebook::react {

winrt::Windows::UI::Color ResolvePlatformColor(const std::vector<std::string> &semanticItems) {
  if (!semanticItems.empty()) {
    for (auto platformColor : semanticItems) {
#ifndef CORE_ABI
      // If XAML is loaded, look in application resources
      if (xaml::TryGetCurrentUwpXamlApplication()) {
        xaml::Media::Brush brush{Microsoft::ReactNative::BrushFromColorObject(platformColor)};
        if (auto scb{brush.try_as<xaml::Media::SolidColorBrush>()}) {
          return scb.Color();
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
        return uiSettings.GetColorValue(uiColor->second);
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
              {"ButtonBorderBrushPointerOver", {0x66, 0x00, 0x00, 0x00}}, // from ControlStrokeColorOnAccentSecondary to
                                                                          // ControlStrokeColorOnAccentDefault
              {"ButtonBorderBrushPressed", {0x00, 0xFF, 0xFF, 0xFF}}, // ControlFillColorTransparent
          };

      auto result = s_xamlBrushes.find(platformColor);
      if (result != s_xamlBrushes.end()) {
        return result->second;
      }
    }
  }

  // Default to transparent color
  return {};
}

} // namespace facebook::react
