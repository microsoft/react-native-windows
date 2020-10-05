// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <React.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace react::uwp {

class AppTheme {
 public:
  AppTheme(
      const Mso::React::IReactContext &context,
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread);

  bool getIsHighContrast();
  folly::dynamic getHighContrastColors();

 private:
  // High Contrast Color helper method
  std::string formatRGB(winrt::Windows::UI::Color ElementColor);

  void fireEvent(std::string const &eventName, folly::dynamic &&eventData);

  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
  bool m_isHighContrast{false};
  folly::dynamic m_highContrastColors;

  winrt::Windows::UI::ViewManagement::AccessibilitySettings m_accessibilitySettings{};
  winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker{};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{};
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
