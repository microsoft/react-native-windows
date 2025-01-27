// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AppThemeModuleUwp.h"

#include <DesktopWindowBridge.h>
#include <IReactContext.h>
#include <ReactPropertyBag.h>
#include <Utils/Helpers.h>
#include <Utils/ThemeUtils.h>
#include <Utils/ValueUtils.h>
#include <XamlUIService.h>
#include <XamlUtils.h>
#include <cxxreact/CxxModule.h>
#include <functional>
#include <memory>
#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace winrt {
using namespace xaml;
using namespace Windows::UI::ViewManagement;
} // namespace winrt

using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<AppThemeHolder>>>
    &AppThemeHolderPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<AppThemeHolder>>> prop{
      L"ReactNative.AppTheme", L"AppThemeHolder"};
  return prop;
}

//
// AppThemeHolder
//

AppThemeHolder::AppThemeHolder(const Mso::React::IReactContext &context) : m_context(&context) {
  NotifyHighContrastChanged();

  if (auto currentApp = xaml::TryGetCurrentUwpXamlApplication()) {
    if (IsWinUI3Island()) {
      m_wmSubscription = SubscribeToWindowMessage(
          ReactNotificationService(m_context->Notifications()), WM_THEMECHANGED, [this](const auto &, const auto &) {
            NotifyHighContrastChanged();
          });
    } else {
      m_highContrastChangedRevoker = m_accessibilitySettings.HighContrastChanged(
          winrt::auto_revoke, [this](const auto &, const auto &) { NotifyHighContrastChanged(); });
    }
  }
}

ReactNativeSpecs::AppThemeSpec_AppThemeData AppThemeHolder::GetConstants() noexcept {
  return m_appThemeData;
}

/*static*/ void AppThemeHolder::InitAppThemeHolder(const Mso::React::IReactContext &context) noexcept {
  auto appThemeHolder = std::make_shared<AppThemeHolder>(context);
  winrt::Microsoft::ReactNative::ReactPropertyBag pb{context.Properties()};
  pb.Set(AppThemeHolderPropertyId(), std::move(appThemeHolder));
}

void AppThemeHolder::SetCallback(
    const React::ReactPropertyBag &propertyBag,
    Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept {
  auto holder = propertyBag.Get(AppThemeHolderPropertyId());

  (*holder)->m_notifyCallback = std::move(callback);
}

void AppThemeHolder::NotifyHighContrastChanged() noexcept {
  m_appThemeData.isHighContrast =
      xaml::TryGetCurrentUwpXamlApplication() ? m_accessibilitySettings.HighContrast() : IsInHighContrastWin32();
  m_appThemeData.highContrastColors.ButtonFaceColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::ButtonFace));
  m_appThemeData.highContrastColors.ButtonTextColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::ButtonText));
  m_appThemeData.highContrastColors.GrayTextColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::GrayText));
  m_appThemeData.highContrastColors.HighlightColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::Highlight));
  m_appThemeData.highContrastColors.HighlightTextColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::HighlightText));
  m_appThemeData.highContrastColors.HotlightColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::Hotlight));
  m_appThemeData.highContrastColors.WindowColor = FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::Window));
  m_appThemeData.highContrastColors.WindowTextColor =
      FormatRGB(m_uiSettings.UIElementColor(winrt::UIElementType::WindowText));

  if (m_notifyCallback) {
    auto writer = MakeJSValueTreeWriter();
    WriteValue(writer, m_appThemeData);
    m_notifyCallback(TakeJSValue(writer).MoveObject());
  }
}

/*static*/ std::string AppThemeHolder::FormatRGB(winrt::Windows::UI::Color ElementColor) {
  char colorChars[8];
  sprintf_s(colorChars, "#%02x%02x%02x", ElementColor.R, ElementColor.G, ElementColor.B);
  return colorChars;
}

void AppTheme::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  AppThemeHolder::SetCallback(
      m_context.Properties(), [weakThis = weak_from_this()](React::JSValueObject &&appThemeInfo) {
        if (auto strongThis = weakThis.lock()) {
          strongThis->m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"highContrastChanged", appThemeInfo);
        }
      });
}

ReactNativeSpecs::AppThemeSpec_AppThemeData AppTheme::GetConstants() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag pb{m_context.Properties()};
  return (*pb.Get(AppThemeHolderPropertyId()))->GetConstants();
}

} // namespace Microsoft::ReactNative
