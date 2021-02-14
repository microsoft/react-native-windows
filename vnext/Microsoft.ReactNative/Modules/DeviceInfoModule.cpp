// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceInfoModule.h"
#include <DesktopWindowBridge.h>
#include <IReactDispatcher.h>
#include <UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#include <Utils/Helpers.h>
#include <XamlUIService.h>
#include <XamlUtils.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>

using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>>
    &DeviceInfoHolderPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>> prop{
      L"ReactNative.DeviceInfo", L"DeviceInfoHolder"};
  return prop;
}

DeviceInfoHolder::DeviceInfoHolder(const Mso::React::IReactContext &context) : m_context(&context) {
  updateDeviceInfo();
}

void DeviceInfoHolder::InitDeviceInfoHolder(
    const Mso::React::IReactContext &context,
    const winrt::Microsoft::ReactNative::ReactPropertyBag &propertyBag) noexcept {
  if (xaml::TryGetCurrentApplication()) {
    auto deviceInfoHolder = std::make_shared<DeviceInfoHolder>(context);

    deviceInfoHolder->updateDeviceInfo();

    propertyBag.Set(DeviceInfoHolderPropertyId(), std::move(deviceInfoHolder));

    if (auto window = xaml::Window::Current()) {
      auto const &coreWindow = window.CoreWindow();

      deviceInfoHolder->m_sizeChangedRevoker =
          coreWindow.SizeChanged(winrt::auto_revoke, [weakHolder = std::weak_ptr(deviceInfoHolder)](auto &&, auto &&) {
            if (auto strongHolder = weakHolder.lock()) {
              strongHolder->updateDeviceInfo();
            }
          });
    } else {
      assert(react::uwp::IsXamlIsland());
      // This is either a WinUI 3 island or a system XAML island
      // system XAML islands have a CoreWindow so we want to use the GetForCurrentView APIs
      // For WinUI 3 islands we require the app to forward window messages as ReactNotifications
    }

    if (!react::uwp::IsWinUI3Island()) {
      // UWP or system XAML island
      auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

      deviceInfoHolder->m_dpiChangedRevoker = displayInfo.DpiChanged(
          winrt::auto_revoke, [weakHolder = std::weak_ptr(deviceInfoHolder)](const auto &, const auto &) {
            if (auto strongHolder = weakHolder.lock()) {
              strongHolder->updateDeviceInfo();
            }
          });
    } else if (auto hwnd = (HWND)XamlUIService::GetIslandWindow(deviceInfoHolder->m_context->Properties())) {
      deviceInfoHolder->m_wmSubscription = SubscribeToWindowMessage(
          context.Notifications(),
          WM_WINDOWPOSCHANGED,
          [weakHolder = std::weak_ptr(deviceInfoHolder)](HWND hwnd, const DesktopWindowMessage &dwm) {
            if (auto strongHolder = weakHolder.lock()) {
              const auto pos = reinterpret_cast<WINDOWPOS *>(dwm.lParam());
              const auto newWidth = (float)(pos->cx);
              const auto newHeight = (float)(pos->cy);
              const auto changed =
                  (strongHolder->m_windowWidth != newWidth) || (strongHolder->m_windowHeight != newHeight);
              strongHolder->m_windowWidth = newWidth;
              strongHolder->m_windowHeight = newHeight;
              if (changed) {
                strongHolder->notifyChanged();
              }
            }
          });
    }
  }
}

void DeviceInfoHolder::notifyChanged() noexcept {
  if (m_notifyCallback) {
    m_notifyCallback(getDimensions());
  }
}

React::JSValueObject DeviceInfoHolder::GetDimensions(const React::ReactPropertyBag &propertyBag) noexcept {
  auto holder = propertyBag.Get(DeviceInfoHolderPropertyId());

  return (*holder)->getDimensions();
}

React::JSValueObject DeviceInfoHolder::getDimensions() noexcept {
  return React::JSValueObject{
      {"windowPhysicalPixels",
       React::JSValueObject{
           {"width", m_windowWidth * m_scale},
           {"height", m_windowHeight * m_scale},
           {"scale", m_scale},
           {"fontScale", m_textScaleFactor},
           {"densityDpi", m_dpi}}},
      {"screenPhysicalPixels",
       React::JSValueObject{
           {"width", m_screenWidth},
           {"height", m_screenHeight},
           {"scale", m_scale},
           {"fontScale", m_textScaleFactor},
           {"densityDpi", m_dpi}}},
  };
}

void DeviceInfoHolder::SetCallback(
    const React::ReactPropertyBag &propertyBag,
    Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept {
  auto holder = propertyBag.Get(DeviceInfoHolderPropertyId());

  (*holder)->m_notifyCallback = std::move(callback);
}

void DeviceInfoHolder::updateDeviceInfo() noexcept {
  if (xaml::Window::Current()) {
    auto const window = xaml::Window::Current().CoreWindow();

    m_windowWidth = window.Bounds().Width;
    m_windowHeight = window.Bounds().Height;
  } else if (auto hwnd = (HWND)XamlUIService::GetIslandWindow(m_context->Properties())) {
    RECT rect{};
    if (CALL_INDIRECT(L"user32.dll", GetWindowRect, hwnd, &rect)) {
      m_windowWidth = (float)(rect.right - rect.left);
      m_windowHeight = (float)(rect.bottom - rect.top);
    }
  }

  winrt::Windows::UI::ViewManagement::UISettings uiSettings;
  m_textScaleFactor = uiSettings.TextScaleFactor();
  if (!react::uwp::IsWinUI3Island()) {
    auto const displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
    m_scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
    m_dpi = displayInfo.LogicalDpi();
    m_screenWidth = displayInfo.ScreenWidthInRawPixels();
    m_screenHeight = displayInfo.ScreenHeightInRawPixels();
    notifyChanged();
  }
}

void DeviceInfo::GetConstants(React::ReactConstantProvider &provider) noexcept {
  provider.Add(L"Dimensions", DeviceInfoHolder::GetDimensions(m_context.Properties()));
}

void DeviceInfo::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  DeviceInfoHolder::SetCallback(
      m_context.Properties(), [weakThis = weak_from_this()](React::JSValueObject &&dimensions) {
        if (auto strongThis = weakThis.lock()) {
          strongThis->m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"didUpdateDimensions", dimensions);
        }
      });
}

} // namespace Microsoft::ReactNative
