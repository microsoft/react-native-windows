// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AppearanceModule.h"
#include <Utils/ThemeUtils.h>
#include <winrt/Windows.UI.ViewManagement.h>

using UISettings = winrt::Windows::UI::ViewManagement::UISettings;

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<bool> &AppearanceCurrentThemePropertyId() noexcept {
  static const React::ReactPropertyId<bool> prop{L"ReactNative.Appearance", L"ApplicationTheme"};
  return prop;
}

void Appearance::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  m_context.UIDispatcher().Post([wkThis = weak_from_this()]() {
    if (auto pThis = wkThis.lock()) {
      pThis->RequeryTheme();
    }
  });

  // UISettings will notify us on a background thread regardless of where we construct it or register for events.
  // Redirect callbacks to the UI thread where we can check app theme.
  m_revoker = m_uiSettings.ColorValuesChanged(
      winrt::auto_revoke,
      [wkThis = weak_from_this(), reactContext](const auto & /*sender*/, const auto & /*args*/) noexcept {
        if (auto pThis = wkThis.lock()) {
          reactContext.UIDispatcher().Post([wkThis]() noexcept {
            if (auto pThis = wkThis.lock()) {
              pThis->RequeryTheme();
            }
          });
        }
      });
}

Appearance::ApplicationTheme CurrentThemeFromUISettings(
    const winrt::Windows::UI::ViewManagement::UISettings &uiSettings) {
  return IsColorLight(uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground))
      ? Appearance::ApplicationTheme::Dark
      : Appearance::ApplicationTheme::Light;
}

Appearance::ApplicationTheme Appearance::GetCurrentTheme() noexcept {
  assert(m_context.UIDispatcher().HasThreadAccess()); // xaml::Application is only accessible on the UI thread
  return CurrentThemeFromUISettings(m_uiSettings);
}

const char *Appearance::ToString(ApplicationTheme theme) noexcept {
  return theme == ApplicationTheme::Dark ? "dark" : "light";
}

void Appearance::RequeryTheme() noexcept {
  auto theme = GetCurrentTheme();
  auto oldTheme = m_context.Properties().Get(AppearanceCurrentThemePropertyId()).value_or(false)
      ? Appearance::ApplicationTheme::Dark
      : Appearance::ApplicationTheme::Light;

  if (oldTheme != theme) {
    appearanceChanged({ToString(theme)});
  }
}

void Appearance::InitOnUIThread(const Mso::React::IReactContext &context) noexcept {
  Appearance::ApplicationTheme theme = Appearance::ApplicationTheme::Light;
  theme = CurrentThemeFromUISettings(winrt::Windows::UI::ViewManagement::UISettings());

  winrt::Microsoft::ReactNative::ReactPropertyBag pb{context.Properties()};
  pb.Set(AppearanceCurrentThemePropertyId(), theme == Appearance::ApplicationTheme::Dark);
}

void Appearance::setColorScheme(std::string style) noexcept {
  // no-op
}

std::optional<std::string> Appearance::getColorScheme() noexcept {
  return ToString(
      m_context.Properties().Get(AppearanceCurrentThemePropertyId()).value_or(false)
          ? Appearance::ApplicationTheme::Dark
          : Appearance::ApplicationTheme::Light);
}

void Appearance::addListener(std::string eventName) noexcept {
  // no-op
}

void Appearance::removeListeners(double count) noexcept {
  // no-op
}

} // namespace Microsoft::ReactNative
