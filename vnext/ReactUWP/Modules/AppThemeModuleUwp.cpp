// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppThemeModuleUwp.h"
#include <iomanip>

#include <Utils\ValueUtils.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::ViewManagement;
}

namespace react { namespace uwp {

//
// AppTheme
//

AppTheme::AppTheme(const std::shared_ptr<IReactInstance>& reactInstance, const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread) : react::windows::AppTheme()
  , m_wkReactInstance(reactInstance)
  , m_queueThread(defaultQueueThread)
{
  m_currentTheme = winrt::Application::Current().RequestedTheme();
  m_isHighContrast = m_accessibilitySettings.HighContrast();
  m_highContrastColors = getHighContrastColors();

  m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {

        folly::dynamic eventData = folly::dynamic::object("highContrastColors", getHighContrastColors())
          ("isHighContrast", getIsHighContrast());

        fireEvent("highContrastChanged", std::move(eventData));
    });

  m_colorValuesChangedRevoker = m_uiSettings.ColorValuesChanged(winrt::auto_revoke,
    [this](const auto&, const auto&) {

      m_queueThread->runOnQueue([this]() {
        if (m_currentTheme != winrt::Application::Current().RequestedTheme() && !m_accessibilitySettings.HighContrast())
        {
          m_currentTheme = winrt::Application::Current().RequestedTheme();

          folly::dynamic eventData = folly::dynamic::object("currentTheme", getCurrentTheme());

          fireEvent("appThemeChanged", std::move(eventData));
        }
      });
  });
}

AppTheme::~AppTheme() = default;

const std::string AppTheme::getCurrentTheme()
{
  return m_currentTheme == winrt::ApplicationTheme::Light ? AppTheme::light : AppTheme::dark;
}

bool AppTheme::getIsHighContrast()
{
  return m_accessibilitySettings.HighContrast();;
}

// Returns the RBG values for the 8 relevant High Contrast elements.
folly::dynamic AppTheme::getHighContrastColors() {
  winrt::Windows::UI::Color ButtonFaceColor = m_uiSettings.UIElementColor(winrt::UIElementType::ButtonFace);
  winrt::Windows::UI::Color ButtonTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::ButtonText);
  winrt::Windows::UI::Color GrayTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::GrayText);
  winrt::Windows::UI::Color HighlightColor = m_uiSettings.UIElementColor(winrt::UIElementType::Highlight);
  winrt::Windows::UI::Color HighlightTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::HighlightText);
  winrt::Windows::UI::Color HotlightColor = m_uiSettings.UIElementColor(winrt::UIElementType::Hotlight);
  winrt::Windows::UI::Color WindowColor = m_uiSettings.UIElementColor(winrt::UIElementType::Window);
  winrt::Windows::UI::Color WindowTextColor = m_uiSettings.UIElementColor(winrt::UIElementType::WindowText);
  
  folly::dynamic rbgValues = folly::dynamic::object("ButtonFaceColor", formatRGB(ButtonFaceColor))
                                                  ("ButtonTextColor", formatRGB(ButtonTextColor))
                                                  ("GrayTextColor", formatRGB(GrayTextColor))
                                                  ("HighlightColor", formatRGB(HighlightColor))
                                                  ("HighlightTextColor", formatRGB(HighlightTextColor))
                                                  ("HotlightColor", formatRGB(HotlightColor))
                                                  ("WindowColor", formatRGB(WindowColor))
                                                  ("WindowTextColor", formatRGB(WindowTextColor));
  return rbgValues;
}

std::string AppTheme::formatRGB(winrt::Windows::UI::Color ElementColor) {
  char colorChars[8];
  sprintf_s(colorChars, "#%02x%02x%02x", ElementColor.R, ElementColor.G, ElementColor.B);
  return colorChars;
}

void AppTheme::fireEvent(std::string const& eventName, folly::dynamic&& eventData)
{
  if (auto instance = m_wkReactInstance.lock())
  {
    instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(eventData)));
  }
}
} } // namespace react::uwp
