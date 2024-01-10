// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <React.h>
#include <ReactNotificationService.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

#include "codegen/NativeAppThemeSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

class AppThemeHolder {
 public:
  AppThemeHolder(const Mso::React::IReactContext &reactContext);

  static void InitAppThemeHolder(const Mso::React::IReactContext &context) noexcept;
  ReactNativeSpecs::AppThemeSpec_AppThemeData GetConstants() noexcept;
  static void SetCallback(
      const React::ReactPropertyBag &propertyBag,
      Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept;

 private:
  // High Contrast Color helper method
  static std::string FormatRGB(winrt::Windows::UI::Color ElementColor);
  void NotifyHighContrastChanged() noexcept;

  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  Mso::Functor<void(React::JSValueObject &&)> m_notifyCallback;
  ReactNativeSpecs::AppThemeSpec_AppThemeData m_appThemeData;
  winrt::Windows::UI::ViewManagement::AccessibilitySettings m_accessibilitySettings{};
  winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker{};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{};
  winrt::Microsoft::ReactNative::ReactNotificationSubscription m_wmSubscription;
};

REACT_MODULE(AppTheme)
struct AppTheme : public std::enable_shared_from_this<AppTheme> {
  using ModuleSpec = ReactNativeSpecs::AppThemeSpec;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::AppThemeSpec_AppThemeData GetConstants() noexcept;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
