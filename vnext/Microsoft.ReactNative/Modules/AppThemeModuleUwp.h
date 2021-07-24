// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <React.h>
#include <ReactNotificationService.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace Microsoft::ReactNative {

class AppTheme : public std::enable_shared_from_this<AppTheme> {
 public:
  AppTheme(
      const Mso::React::IReactContext &reactContext,
      const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread);

  bool getIsHighContrast() const;
  folly::dynamic getHighContrastColors() const;

 private:
  // High Contrast Color helper method
  static std::string formatRGB(winrt::Windows::UI::Color ElementColor);
  void NotifyHighContrastChanged() const;
  void fireEvent(std::string const &eventName, folly::dynamic &&eventData) const;

  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::shared_ptr<facebook::react::MessageQueueThread> m_queueThread;
  bool m_isHighContrast{false};
  folly::dynamic m_highContrastColors;
  winrt::Windows::UI::ViewManagement::AccessibilitySettings m_accessibilitySettings{};
  winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker{};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{};
  winrt::Microsoft::ReactNative::ReactNotificationSubscription m_wmSubscription;
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

} // namespace Microsoft::ReactNative
