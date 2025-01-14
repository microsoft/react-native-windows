
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Theme.h"

#include "Composition.CustomResourceResult.g.h"
#include <Fabric/Composition/CompositionUIService.h>
#include <Utils/ThemeUtils.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.Composition.h"

#ifndef CORE_ABI
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
#include <XamlUtils.h>

#endif // CORE_ABI

#include "Composition.Theme.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct CustomResourceResult : CustomResourceResultT<CustomResourceResult> {
  winrt::Windows::Foundation::IInspectable Resource() const noexcept {
    return m_resource;
  }
  void Resource(const winrt::Windows::Foundation::IInspectable &value) noexcept {
    m_resource = value;
  }

  winrt::hstring AlternateResourceId() const noexcept {
    return m_alternateResourceId;
  }
  void AlternateResourceId(winrt::hstring value) noexcept {
    m_alternateResourceId = value;
  }

  winrt::Windows::Foundation::IInspectable m_resource{nullptr};
  winrt::hstring m_alternateResourceId;
};

Theme::Theme(
    const winrt::Microsoft::ReactNative::ReactContext &reactContext,
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &customResourceLoader) noexcept
    : m_customResourceLoader(customResourceLoader) {
  m_compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          reactContext.Properties().Handle());

  if (customResourceLoader) {
    m_resourceChangedRevoker = customResourceLoader.ResourcesChanged(
        winrt::auto_revoke,
        [this](
            winrt::Windows::Foundation::IInspectable const & /* sender */,
            winrt::Windows::Foundation::IInspectable const & /* args */) { ClearCacheAndRaiseChangedEvent(); });
  }

  // UISettings will notify us on a background thread regardless of where we construct it or register for events.
  // Redirect callbacks to the UI thread
  m_colorValuesChangedRevoker = m_uisettings.ColorValuesChanged(
      winrt::auto_revoke, [wkThis = get_weak(), reactContext](const auto & /*sender*/, const auto & /*args*/) noexcept {
        if (auto pThis = wkThis.get()) {
          reactContext.UIDispatcher().Post([wkThis]() noexcept {
            if (auto pThis = wkThis.get()) {
              pThis->ClearCacheAndRaiseChangedEvent();
            }
          });
        }
      });

  m_highContrast = ::Microsoft::ReactNative::IsInHighContrastWin32();
  m_darkTheme = ::Microsoft::ReactNative::IsColorLight(
      m_uisettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground));
}

void Theme::ClearCacheAndRaiseChangedEvent() noexcept {
  m_colorCache.clear();
  m_platformColorBrushCache.clear();
  m_colorBrushCache.clear();
  m_darkTheme = ::Microsoft::ReactNative::IsColorLight(
      m_uisettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground));
  m_highContrast = ::Microsoft::ReactNative::IsInHighContrastWin32();
  m_themeChangedEvent(*this, nullptr);
}

Theme::Theme() noexcept : m_emptyTheme(true) {}

bool Theme::TryGetPlatformColor(winrt::hstring platformColor, winrt::Windows::UI::Color &color) noexcept {
  return TryGetPlatformColor(winrt::to_string(platformColor), color);
}

bool Theme::TryGetPlatformColor(const std::string &platformColor, winrt::Windows::UI::Color &color) noexcept {
  // Future: This could take into account the system theme

  if (m_emptyTheme)
    return false;

  if (auto cachedEntry = m_colorCache.find(platformColor); cachedEntry != m_colorCache.end()) {
    if (!cachedEntry->second.first) {
      return false;
    }

    color = cachedEntry->second.second;
    return true;
  }

  if (m_customResourceLoader) {
    auto result = winrt::make<CustomResourceResult>();
    m_customResourceLoader.GetResource(
        winrt::to_hstring(platformColor), winrt::Microsoft::ReactNative::Composition::ResourceType::Color, result);
    if (!result.AlternateResourceId().empty()) {
      return TryGetPlatformColor(winrt::to_string(result.AlternateResourceId()), color);
    }
    if (auto resource = result.Resource()) {
      color = winrt::unbox_value<winrt::Windows::UI::Color>(resource);
      m_colorCache[platformColor] = std::make_pair(true, color);
      return true;
    }
  }

#ifndef CORE_ABI
  // If XAML is loaded, look in application resources
  if (xaml::TryGetCurrentUwpXamlApplication()) {
    const auto appResources{xaml::Application::Current().Resources()};
    const auto boxedResourceName{winrt::box_value(winrt::to_hstring(platformColor))};
    if (appResources.HasKey(boxedResourceName)) {
      winrt::IInspectable resource{appResources.Lookup(boxedResourceName)};

      if (auto brush = resource.try_as<xaml::Media::SolidColorBrush>()) {
        color = brush.Color();
        m_colorCache[platformColor] = std::make_pair(true, brush.Color());
        return true;
      } else if (auto maybeColor = resource.try_as<winrt::Windows::UI::Color>()) {
        color = maybeColor.value();
        m_colorCache[platformColor] = std::make_pair(true, color);
        return true;
      }
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

  if (platformColor == "AccentDark1@90" && TryGetPlatformColor("AccentDark1", color)) {
    color.A = static_cast<uint8_t>(static_cast<float>(color.A) * 0.9f);
    m_colorCache[platformColor] = std::make_pair(true, color);
    return true;
  }

  if (platformColor == "AccentDark1@80" && TryGetPlatformColor("AccentDark1", color)) {
    color.A = static_cast<uint8_t>(static_cast<float>(color.A) * 0.8f);
    m_colorCache[platformColor] = std::make_pair(true, color);
    return true;
  }

  auto uiColor = s_uiColorTypes.find(platformColor);
  if (uiColor != s_uiColorTypes.end()) {
    auto uiSettings{winrt::Windows::UI::ViewManagement::UISettings()};
    color = uiSettings.GetColorValue(uiColor->second);
    auto pair = std::make_pair(true, color);
    m_colorCache[platformColor] = pair;
    return true;
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
    color = uiSettings.UIElementColor(uiElement->second);
    m_colorCache[platformColor] = std::make_pair(true, color);
    return true;
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
          {"ControlElevationBorder",
           "ControlStrokeColorSecondary"}, // TODO ControlElevationBorderBrush is actually gradient brush
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
          {"AccentFillColorSecondary", "AccentDark1@90"}, // SystemAccentColorDark1 + Opacity 0.9
          {"AccentFillColorTertiary", "AccentDark1@80"}, // SystemAccentColorDark1 + Opacity 0.8
          {"CircleElevationBorder", "ControlStrokeColorDefault"}, // TODO is actually a linear brush
          {"ProgressRingForegroundTheme", "AccentFillColorDefault"},
          {"TextControlForeground", "TextFillColorPrimary"},
          {"ScrollBarButtonBackground", "SubtleFillColorTransparent"},
          {"ScrollBarButtonBackgroundPointerOver", "SubtleFillColorSecondary"},
          {"ScrollBarButtonBackgroundPressed", "ControlStrongFillColorDefault"},
          {"ScrollBarButtonBackgroundDisabled", "SubtleFillColorTransparent"},
          {"ScrollBarButtonArrowForeground", "ControlStrongFillColorDefault"},
          {"ScrollBarButtonArrowForegroundPointerOver", "TextFillColorSecondary"},
          {"ScrollBarButtonArrowForegroundPressed", "TextFillColorSecondary"},
          {"ScrollBarButtonArrowForegroundDisabled", "ControlStrongFillColorDisabled"},
          {"ScrollBarThumbFill", "ControlStrongFillColorDefault"},
          {"ScrollBarThumbFillPointerOver", "ControlStrongFillColorDefault"},
          {"ScrollBarThumbFillPressed", "ControlStrongFillColorDefault"},
          {"ScrollBarThumbFillDisabled", "ControlStrongFillColorDisabled"},
          {"ScrollBarTrackFill",
           "AcrylicInAppFillColorDefault"}, // TODO make AcrylicInAppFillColorDefault a real acrylic brush
          {"ToolTipBackground", "SystemChromeMediumLowColor"},
          {"ToolTipForeground", "SystemControlForegroundBaseHighColor"},
          {"ToolTipBorderBrush", "SystemControlTransientBorderColor"}};

  static std::unordered_map<std::string, winrt::Windows::UI::Color, std::hash<std::string_view>, std::equal_to<>>
      s_lightColors = {
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
          {"SubtleFillColorTransparent", {0x00, 0x00, 0x00, 0x00}},
          {"SubtleFillColorSecondary", {0x09, 0x00, 0x00, 0x00}},
          {"ControlStrongFillColorDefault", {0x72, 0x00, 0x00, 0x00}},
          {"ControlStrongFillColorDisabled", {0x51, 0x00, 0x00, 0x00}},
          {"AcrylicInAppFillColorDefault", {0x9E, 0xFF, 0xFF, 0xFF}},
          {"SystemChromeMediumLowColor", {0xFF, 0xF2, 0xF2, 0xF2}},
          {"SystemControlForegroundBaseHighColor", {0xFF, 0x00, 0x00, 0x00}},
          {"SystemControlTransientBorderColor", {0x24, 0x00, 0x00, 0x00}},
          {"FocusVisualPrimary", {0xFF, 0x00, 0x00, 0x00}},
          {"FocusVisualSecondary", {0x99, 0xFF, 0xFF, 0xFF}}};

  static std::unordered_map<std::string, winrt::Windows::UI::Color, std::hash<std::string_view>, std::equal_to<>>
      s_darkColors = {
          {"SolidBackgroundFillColorBase", {0xFF, 0x20, 0x20, 0x20}},
          {"ControlFillColorDefault", {0x0F, 0xFF, 0xFF, 0xFF}},
          {"ControlFillColorSecondary", {0x15, 0xFF, 0xFF, 0xFF}},
          {"ControlFillColorTertiary", {0x08, 0xFF, 0xFF, 0xFF}},
          {"ControlFillColorDisabled", {0x0B, 0xFF, 0xFF, 0xFF}},
          {"ControlFillColorTransparent", {0x00, 0xFF, 0xFF, 0xFF}},
          {"ControlStrokeColorDefault", {0x12, 0xFF, 0xFF, 0xFF}},
          {"ControlStrokeColorSecondary", {0x18, 0xFF, 0xFF, 0xFF}},
          {"ControlStrokeColorOnAccentSecondary", {0x23, 0x00, 0x00, 0x00}},
          {"TextFillColorPrimary", {0xFF, 0xFF, 0xFF, 0xFF}},
          {"TextFillColorSecondary", {0xC5, 0xFF, 0xFF, 0xFF}},
          {"TextFillColorDisabled", {0x5D, 0xFF, 0xFF, 0xFF}},
          {"ControlAltFillColorTertiary", {0x0B, 0xFF, 0xFF, 0xFF}},
          {"ControlAltFillColorQuarternary", {0x12, 0xFF, 0xFF, 0xFF}},
          {"ControlAltFillColorDisabled", {0x00, 0xFF, 0xFF, 0xFF}},
          {"ControlStrongStrokeColorDefault", {0x8B, 0xFF, 0xFF, 0xFF}},
          {"ControlStrongStrokeColorDisabled", {0x28, 0xFF, 0xFF, 0xFF}},
          {"AccentFillColorDisabled", {0x28, 0xFF, 0xFF, 0xFF}},
          {"TextOnAccentFillColorPrimary", {0xFF, 0x00, 0x00, 0x00}},
          {"TextOnAccentFillColorDisabled", {0x87, 0xFF, 0xFF, 0xFF}},
          {"ControlAltFillColorSecondary", {0x19, 0x00, 0x00, 0x00}},
          {"SubtleFillColorTransparent", {0x00, 0x00, 0x00, 0x00}},
          {"SubtleFillColorSecondary", {0x0F, 0xFF, 0xFF, 0xFF}},
          {"ControlStrongFillColorDefault", {0x8B, 0xFF, 0xFF, 0xFF}},
          {"ControlStrongFillColorDisabled", {0x3F, 0xFF, 0xFF, 0xFF}},
          {"AcrylicInAppFillColorDefault", {0x9E, 0x00, 0x00, 0x00}},
          {"SystemChromeMediumLowColor", {0xFF, 0x2B, 0x2B, 0x2B}},
          {"SystemControlForegroundBaseHighColor", {0xFF, 0xFF, 0xFF, 0xFF}},
          {"SystemControlTransientBorderColor", {0x5C, 0x00, 0x00, 0x00}},
          {"FocusVisualPrimary", {0xFF, 0xFF, 0xFF, 0xFF}},
          {"FocusVisualSecondary", {0x99, 0x00, 0x00, 0x00F}}};

  static std::unordered_map<
      std::string,
      std::pair<winrt::Windows::UI::ViewManagement::UIElementType, winrt::Windows::UI::Color>,
      std::hash<std::string_view>,
      std::equal_to<>>
      s_contrastColors = {
          {"SolidBackgroundFillColorBase", {winrt::Windows::UI::ViewManagement::UIElementType::Window, {}}},
          {"ControlFillColorDefault", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlFillColorSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlFillColorTertiary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlFillColorTransparent",
           {(winrt::Windows::UI::ViewManagement::UIElementType)-1, {0x00, 0xFF, 0xFF, 0xFF}}},
          {"SubtleFillColorTransparent",
           {(winrt::Windows::UI::ViewManagement::UIElementType)-1, {0x00, 0xFF, 0xFF, 0xFF}}},
          {"ControlStrokeColorDefault", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"ControlStrokeColorSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"ControlStrokeColorOnAccentSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"TextFillColorPrimary", {winrt::Windows::UI::ViewManagement::UIElementType::WindowText, {}}},
          {"TextFillColorSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::WindowText, {}}},
          {"TextFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::GrayText, {}}},
          {"ControlAltFillColorTertiary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlAltFillColorQuarternary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlAltFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlStrongStrokeColorDefault", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"ControlStrongStrokeColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"AccentFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::Window, {}}},
          {"TextOnAccentFillColorPrimary", {winrt::Windows::UI::ViewManagement::UIElementType::WindowText, {}}},
          {"TextOnAccentFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::GrayText, {}}},
          {"ControlAltFillColorSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"SubtleFillColorSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlStrongFillColorDefault", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"ControlStrongFillColorDisabled", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"SystemChromeMediumLowColor", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}},
          {"SystemControlForegroundBaseHighColor", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"SystemControlTransientBorderColor", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"FocusVisualPrimary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonText, {}}},
          {"FocusVisualSecondary", {winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace, {}}}};

  auto alias = s_xamlAliasedColors.find(platformColor);
  if (alias != s_xamlAliasedColors.end()) {
    return TryGetPlatformColor(alias->second, color);
  }

  if (m_highContrast) {
    auto result = s_contrastColors.find(platformColor);
    if (result != s_contrastColors.end()) {
      if (result->second.first == (winrt::Windows::UI::ViewManagement::UIElementType)-1) {
        color = result->second.second;
      } else {
        color = m_uisettings.UIElementColor(result->second.first);
      }
      m_colorCache[platformColor] = std::make_pair(true, color);
      return true;
    }
  } else {
    auto &builtInColors = m_darkTheme ? s_darkColors : s_lightColors;
    auto result = builtInColors.find(platformColor);
    if (result != builtInColors.end()) {
      color = result->second;
      m_colorCache[platformColor] = std::make_pair(true, color);
      return true;
    }
  }

  m_colorCache[platformColor] = std::make_pair(false, winrt::Windows::UI::Color{});
  return false;
}

winrt::Windows::UI::Color Theme::Color(const facebook::react::Color &color) noexcept {
  winrt::Windows::UI::Color clr;
  if (!color.m_platformColor.empty()) {
    for (auto platformColor : color.m_platformColor) {
      if (TryGetPlatformColor(platformColor, clr)) {
        return clr;
      }
    }
  }
  return color.m_color;
}

winrt::Windows::UI::Color Theme::PlatformColor(const std::string &platformColor) noexcept {
  winrt::Windows::UI::Color color;
  if (TryGetPlatformColor(platformColor, color)) {
    return color;
  }

  assert(m_emptyTheme);
  return {0, 0, 0, 0}; // Transparent
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Theme::InternalPlatformBrush(
    winrt::hstring platformColor) noexcept {
  return PlatformBrush(winrt::to_string(platformColor));
}

#ifdef USE_WINUI3
winrt::Microsoft::UI::Composition::CompositionBrush Theme::PlatformBrush(winrt::hstring platformColor) noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerBrush(
      PlatformBrush(winrt::to_string(platformColor)));
}
#endif

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Theme::PlatformBrush(
    const std::string &platformColor) noexcept {
  if (m_emptyTheme)
    return nullptr;

  if (auto cachedEntry = m_platformColorBrushCache.find(platformColor); cachedEntry != m_platformColorBrushCache.end())
    return cachedEntry->second;

  winrt::Windows::UI::Color color;
  if (TryGetPlatformColor(platformColor, color)) {
    auto brush = m_compositionContext.CreateColorBrush(color);
    m_platformColorBrushCache[platformColor] = brush;
    return brush;
  }
  return nullptr;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Theme::Brush(
    const facebook::react::Color &color) noexcept {
  if (m_emptyTheme)
    return nullptr;

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

D2D1::ColorF Theme::D2DColor(const facebook::react::Color &color) noexcept {
  auto c = Color(color);
  return {c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f};
}

D2D1::ColorF Theme::D2DPlatformColor(const std::string &platformColor) noexcept {
  auto c = PlatformColor(platformColor);
  return {c.R / 255.0f, c.G / 255.0f, c.B / 255.0f, c.A / 255.0f};
}

bool Theme::IsEmpty() const noexcept {
  return m_emptyTheme;
}

winrt::event_token Theme::ThemeChanged(
    winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const &handler) noexcept {
  return m_themeChangedEvent.add(handler);
}
void Theme::ThemeChanged(winrt::event_token const &token) noexcept {
  m_themeChangedEvent.remove(token);
}

static const winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::Composition::Theme>
    &ThemePropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Microsoft::ReactNative::Composition::Theme> prop{
      L"ReactNative.Composition", L"Theme"};
  return prop;
}

static const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader>
    &ThemeResourcesPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader>
      prop{L"ReactNative.Composition", L"ThemeResources"};
  return prop;
}

winrt::Microsoft::ReactNative::Composition::Theme Theme::EmptyTheme() noexcept {
  static winrt::Microsoft::ReactNative::Composition::Theme s_emptyTheme{nullptr};
  if (!s_emptyTheme) {
    s_emptyTheme = winrt::make<Theme>();
  }
  return s_emptyTheme;
}

/*static*/ winrt::Microsoft::ReactNative::Composition::Theme Theme::GetDefaultTheme(
    const winrt::Microsoft::ReactNative::IReactContext &context) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(context.Properties())
      .GetOrCreate(ThemePropertyId(), [context]() {
        return winrt::make<Theme>(
            context,
            winrt::Microsoft::ReactNative::ReactPropertyBag(context.Properties()).Get(ThemeResourcesPropertyId()));
      });
}

/*static*/ void Theme::SetDefaultResources(
    const winrt::Microsoft::ReactNative::ReactInstanceSettings &settings,
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag properties(settings.Properties());
  properties.Set(ThemeResourcesPropertyId(), resources);
  // If a default theme has already been created - we need to update it with the new resources
  if (auto theme = properties.Get(ThemePropertyId())) {
    winrt::get_self<Theme>(theme)->UpdateCustomResources(resources);
  }
}

void Theme::UpdateCustomResources(
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept {
  m_customResourceLoader = resources;
  ClearCacheAndRaiseChangedEvent();
}

IReactPropertyNamespace ThemeNamespace() noexcept {
  static IReactPropertyNamespace value = ReactPropertyBagHelper::GetNamespace(L"ReactNative.Theme");
  return value;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
