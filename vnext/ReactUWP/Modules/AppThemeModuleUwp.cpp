// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppThemeModuleUwp.h"

#include <Utils\ValueUtils.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
  using namespace Windows::UI::Xaml;
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
  return m_currentTheme == winrt::ApplicationTheme::Light ? "light" : "dark";
}

void AppTheme::fireEvent(std::string const& eventName, folly::dynamic const& eventData)
{
  if (auto instance = m_wkReactInstance.lock())
  {
    instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(eventData)));
  }
}

} } // namespace react::uwp
