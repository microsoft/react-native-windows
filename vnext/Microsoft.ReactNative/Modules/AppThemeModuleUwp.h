// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace react::uwp {

class AppTheme {
 public:
  static constexpr const char *Dark = "dark";
  static constexpr const char *Light = "light";

  AppTheme(
      const std::shared_ptr<IReactInstance> &reactInstance,
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread);

  std::string getCurrentTheme();
  bool getIsHighContrast();
  folly::dynamic getHighContrastColors();

 private:
  // High Contrast Color helper method
  std::string formatRGB(winrt::Windows::UI::Color ElementColor);

  void fireEvent(std::string const &eventName, folly::dynamic &&eventData);

  std::weak_ptr<IReactInstance> m_wkReactInstance;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
  xaml::ApplicationTheme m_currentTheme{xaml::ApplicationTheme::Light};
  bool m_isHighContrast{false};
  folly::dynamic m_highContrastColors;

  winrt::Windows::UI::ViewManagement::AccessibilitySettings m_accessibilitySettings{};
  winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker{};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{};
  winrt::Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker m_colorValuesChangedRevoker{};
};

class AppThemeModule : public facebook::xplat::module::CxxModule {
 public:
  static constexpr const char *Name = "RTCAppTheme";

  AppThemeModule(std::shared_ptr<AppTheme> &&appTheme);

  // CxxModule
  std::string getName() override {
    return Name;
  };
  auto getConstants() -> std::map<std::string, folly::dynamic> override;
  auto getMethods() -> std::vector<Method> override;

 private:
  std::shared_ptr<AppTheme> m_appTheme;
};

} // namespace react::uwp
