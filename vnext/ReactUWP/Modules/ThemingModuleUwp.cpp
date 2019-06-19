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
    m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        fireEvent(ThemingEvent::HighContrast, m_accessibilitySettings.HighContrastScheme());
    });

    m_colorValuesChangedRevoker = m_uiSettings.ColorValuesChanged(winrt::auto_revoke,
      [this](const auto&, const auto&) {
        m_queueThread->runOnQueue([this]() {
          if (m_currentTheme != winrt::Application::Current().RequestedTheme())
          {
            m_currentTheme = winrt::Application::Current().RequestedTheme();
            fireEvent(ThemingEvent::Theme, winrt::to_hstring(getTheme()));
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
    return m_accessibilitySettings.HighContrast();
  }

  void Theming::fireEvent(ThemingEvent event, winrt::hstring const& args)
  {
    std::string paramsName{ event == ThemingEvent::Theme ? "platform_theme" : "highContrastScheme" };
    std::string eventName{ event == ThemingEvent::Theme ? "themeDidChange" : "highContrastDidChange" };

    auto instance = m_wkReactInstance.lock();
    if (instance)
    {
      folly::dynamic parameters = folly::dynamic::object(paramsName, winrt::to_string(args));
      instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(parameters)));
    }
  }
}
} // namespace react::uwp