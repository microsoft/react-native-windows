// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <Modules/AppThemeModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace react {
namespace uwp {

class AppTheme : public react::windows::AppTheme {
 public:
  AppTheme(
      const std::shared_ptr<IReactInstance> &reactInstance,
      const std::shared_ptr<facebook::react::MessageQueueThread>
          &defaultQueueThread);
  virtual ~AppTheme();

  const std::string getCurrentTheme() override;
  bool getIsHighContrast() override;

  static void uiThreadAvailable();

 private:
  // High Contrast Color helper methods
  folly::dynamic getHighContrastColors();
  std::string formatRGB(winrt::Windows::UI::Color ElementColor);

  void fireEvent(std::string const &eventName, folly::dynamic &&eventData);
  void connectNativeEventHandlers();

  std::weak_ptr<IReactInstance> m_wkReactInstance;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
  winrt::Windows::UI::Xaml::ApplicationTheme m_currentTheme{
      winrt::Windows::UI::Xaml::ApplicationTheme::Light};
  bool m_isHighContrast;
  folly::dynamic m_highContrastColors;

  winrt::Windows::UI::ViewManagement::AccessibilitySettings
      m_accessibilitySettings{};
  winrt::Windows::UI::ViewManagement::AccessibilitySettings::
      HighContrastChanged_revoker m_highContrastChangedRevoker{};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{};
  winrt::Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker
      m_colorValuesChangedRevoker{};

  static AppTheme *s_currentInstance;
};

} // namespace uwp
} // namespace react
