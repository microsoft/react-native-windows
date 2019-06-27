// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Modules/AppThemeModule.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <cxxreact/MessageQueueThread.h>
#include <IReactInstance.h>

namespace react {
  namespace uwp {

    class AppTheme : public react::windows::AppTheme
    {
    public:
      AppTheme(const std::shared_ptr<IReactInstance>& reactInstance, const std::shared_ptr<facebook::react::MessageQueueThread>& defaultQueueThread);
      virtual ~AppTheme();

      const std::string getCurrentTheme() override;
      bool getIsHighContrast() override;

    private:
      // RGB helper methods
      folly::dynamic getHighContrastRGBValues();
      std::string formatRGB(winrt::Windows::UI::Color ElementColor);

      void fireEvent(std::string const& eventName, folly::dynamic const& eventData);

      std::weak_ptr<IReactInstance> m_wkReactInstance;
      std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
      winrt::Windows::UI::Xaml::ApplicationTheme m_currentTheme{ winrt::Windows::UI::Xaml::ApplicationTheme::Light };
      bool m_isHighContrast;
      folly::dynamic m_RGBValues;

      winrt::Windows::UI::ViewManagement::AccessibilitySettings m_accessibilitySettings{ };
      winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker{ };
      winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{ };
      winrt::Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker m_colorValuesChangedRevoker{ };
    };

  }
} // namespace react::uwp
