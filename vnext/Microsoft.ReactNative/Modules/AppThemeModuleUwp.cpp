// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AppThemeModuleUwp.h"

#include <DesktopWindowBridge.h>
#include <IReactContext.h>
#include <ReactPropertyBag.h>
#include <Utils/Helpers.h>
#include <Utils/ValueUtils.h>
#include <XamlUIService.h>
#include <XamlUtils.h>
#include <cxxreact/CxxModule.h>
#include <functional>
#include <memory>
#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace winrt {
using namespace xaml;
using namespace Windows::UI::ViewManagement;
} // namespace winrt

using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {

//
// AppTheme
//

AppTheme::AppTheme(
    const Mso::React::IReactContext &context,
    const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread)
    : m_context(&context), m_queueThread(defaultQueueThread) {
  if (auto currentApp = xaml::TryGetCurrentApplication()) {
    m_isHighContrast = getIsHighContrast();
    m_highContrastColors = getHighContrastColors();

    if (IsWinUI3Island()) {
      m_wmSubscription = SubscribeToWindowMessage(
          ReactNotificationService(m_context->Notifications()), WM_THEMECHANGED, [this](const auto &, const auto &) {
            NotifyHighContrastChanged();
          });
    } else {
      m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(
          winrt::auto_revoke, [this](const auto &, const auto &) { NotifyHighContrastChanged(); });
    }
  }
}

bool AppTheme::getIsHighContrast() const {
  return m_accessibilitySettings.HighContrast();
}

// Returns the RBG values for the 8 relevant High Contrast elements.
folly::dynamic AppTheme::getHighContrastColors() const {
  winrt::Windows::UI::Color ButtonFaceColor = m_uiSettings.UIElementColor(winrt::UIElementType::ButtonFace);
  winrt::Windows::UI::Color ButtonTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::ButtonText);
  winrt::Windows::UI::Color GrayTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::GrayText);
  winrt::Windows::UI::Color HighlightColor = m_uiSettings.UIElementColor(winrt::UIElementType::Highlight);
  winrt::Windows::UI::Color HighlightTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::HighlightText);
  winrt::Windows::UI::Color HotlightColor = m_uiSettings.UIElementColor(winrt::UIElementType::Hotlight);
  winrt::Windows::UI::Color WindowColor = m_uiSettings.UIElementColor(winrt::UIElementType::Window);
  winrt::Windows::UI::Color WindowTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::WindowText);

  folly::dynamic rbgValues = folly::dynamic::object("ButtonFaceColor", formatRGB(ButtonFaceColor))(
      "ButtonTextColor", formatRGB(ButtonTextColor))("GrayTextColor", formatRGB(GrayTextColor))(
      "HighlightColor", formatRGB(HighlightColor))("HighlightTextColor", formatRGB(HighlightTextColor))(
      "HotlightColor", formatRGB(HotlightColor))("WindowColor", formatRGB(WindowColor))(
      "WindowTextColor", formatRGB(WindowTextColor));
  return rbgValues;
}

/*static*/ std::string AppTheme::formatRGB(winrt::Windows::UI::Color ElementColor) {
  char colorChars[8];
  sprintf_s(colorChars, "#%02x%02x%02x", ElementColor.R, ElementColor.G, ElementColor.B);
  return colorChars;
}

void AppTheme::fireEvent(std::string const &eventName, folly::dynamic &&eventData) const {
  m_context->CallJSFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(eventData)));
}

void AppTheme::NotifyHighContrastChanged() const {
  folly::dynamic eventData =
      folly::dynamic::object("highContrastColors", getHighContrastColors())("isHighContrast", getIsHighContrast());
  this->

      fireEvent("highContrastChanged", std::move(eventData));
}

AppThemeModule::AppThemeModule(std::shared_ptr<AppTheme> &&appTheme) : m_appTheme(std::move(appTheme)) {}

auto AppThemeModule::getConstants() -> std::map<std::string, folly::dynamic> {
  return {
      {"initialHighContrast", folly::dynamic{m_appTheme->getIsHighContrast()}},
      {"initialHighContrastColors", folly::dynamic{m_appTheme->getHighContrastColors()}}};
}

auto AppThemeModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method> {
  return {};
}

} // namespace Microsoft::ReactNative
