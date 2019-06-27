// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ThemingModuleUwp.h"
#include <iomanip>
#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Windows::UI::Xaml;
}

namespace react {
  namespace uwp {

  //
  // Theming
  //

  Theming::Theming(const std::shared_ptr<IReactInstance>& reactInstance, const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread) : react::windows::PlatformTheme()
    , m_wkReactInstance(reactInstance)
    , m_queueThread(defaultQueueThread)
  {
    m_currentTheme = winrt::Application::Current().RequestedTheme();
    m_isHighContrast = m_accessibilitySettings.HighContrast();
    m_RGBValues = getHighContrastRGBValues();

    m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        folly::dynamic eventData = folly::dynamic::object("highContrastRGBValues", getHighContrastRGBValues())
          ("isHighContrast", getIsHighContrast());
        fireEvent("highContrastChanged", std::move(eventData));// fire event with information that it's a high contrast change and what the scheme is
    });

    m_colorValuesChangedRevoker = m_uiSettings.ColorValuesChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        m_queueThread->runOnQueue([this]() {
          if (m_currentTheme != winrt::Application::Current().RequestedTheme())
          {
            m_currentTheme = winrt::Application::Current().RequestedTheme();
            folly::dynamic eventData = folly::dynamic::object("currentTheme", getCurrentTheme());
            fireEvent("appThemeChanged", std::move(eventData));
          }
        });
    });
  }

  // Returns the RBG values for the 8 relevant High Contrast elements.
  folly::dynamic Theming::getHighContrastRGBValues() {
    winrt::Windows::UI::Color ButtonFaceColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::ButtonFace);
    winrt::Windows::UI::Color ButtonTextColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::ButtonText);
    winrt::Windows::UI::Color GrayTextColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::GrayText);
    winrt::Windows::UI::Color HighlightColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::Highlight);
    winrt::Windows::UI::Color HighlightTextColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::HighlightText);
    winrt::Windows::UI::Color HotlightColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::Hotlight);
    winrt::Windows::UI::Color WindowColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::Window);
    winrt::Windows::UI::Color WindowTextColor = m_uiSettings.UIElementColor(winrt::Windows::UI::ViewManagement::UIElementType::WindowText);
    folly::dynamic rbgValues = folly::dynamic::object("ButtonFaceRGB", formatRGB(ButtonFaceColor))
                                                    ("ButtonTextRGB", formatRGB(ButtonTextColor))
                                                    ("GrayTextRGB", formatRGB(GrayTextColor))
                                                    ("HighlightRGB", formatRGB(HighlightColor))
                                                    ("HightlightTextRGB", formatRGB(HighlightTextColor))
                                                    ("HotlightRGB", formatRGB(HotlightColor))
                                                    ("WindowRGB", formatRGB(WindowColor))
                                                    ("WindowTextRGB", formatRGB(WindowTextColor));
    return rbgValues;
  }

  std::string Theming::formatRGB(winrt::Windows::UI::Color ElementColor) {
    std::stringstream RGBString;
    RGBString << "#" << std::setfill('0') << std::setw(3) << (int)ElementColor.R << std::setfill('0') << std::setw(3) << (int)ElementColor.G << std::setfill('0') << std::setw(3) << (int)ElementColor.B;
    return RGBString.str();
  }

  Theming::~Theming() = default;

  const std::string Theming::getCurrentTheme()
  {
    return m_currentTheme == winrt::ApplicationTheme::Light ? "light" : "dark";
  }

  bool Theming::getIsHighContrast()
  {
    return m_accessibilitySettings.HighContrast();;
  }

  void Theming::fireEvent(std::string const& eventName, folly::dynamic const& eventData)
  {
    if (auto instance = m_wkReactInstance.lock())
    {
      instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(eventData)));
    }
  }
}
} // namespace react::uwp
