// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ThemingModuleUwp.h"

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

    m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        if (m_isHighContrast != m_accessibilitySettings.HighContrast()) {
          m_isHighContrast = m_accessibilitySettings.HighContrast();
        }
        const std::string hcScheme{ getIsHighContrast() ?                     // Is high contrast being turned on or off?
    winrt::to_string(m_accessibilitySettings.HighContrastScheme()) : "None" };
        fireEvent(ThemingEvent::HighContrast, hcScheme); // fire event with information that it's a high contrast change and what the scheme is.
    });

    m_colorValuesChangedRevoker = m_uiSettings.ColorValuesChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        m_queueThread->runOnQueue([this]() {
          if (m_currentTheme != winrt::Application::Current().RequestedTheme())
          {
            m_currentTheme = winrt::Application::Current().RequestedTheme();
            fireEvent(ThemingEvent::Theme, getTheme());
          }
        });
    });
  }

  Theming::~Theming() = default;

  const std::string Theming::getTheme()
  {
    return m_currentTheme == winrt::ApplicationTheme::Light ? "light" : "dark";
  }

  bool Theming::getIsHighContrast()
  {
    return m_isHighContrast;
  }

  void Theming::fireEvent(ThemingEvent event, std::string const& args)
  {
    std::string paramsName{ event == ThemingEvent::Theme ? "platform_theme" : "highContrastScheme" };
    std::string eventName{ event == ThemingEvent::Theme ? "themeDidChange" : "highContrastDidChange" };

    auto instance = m_wkReactInstance.lock();
    if (instance)
    {
      folly::dynamic parameters = folly::dynamic::object(paramsName, args); // For high contrast args are the scheme.
      instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(parameters)));
    }
  }
}
} // namespace react::uwp