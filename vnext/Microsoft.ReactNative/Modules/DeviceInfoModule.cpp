// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceInfoModule.h"
#include <IReactDispatcher.h>
#include <UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#include <Utils/Helpers.h>
#include <XamlUtils.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>>
    &DeviceInfoHolderPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>> prop{
      L"ReactNative.DeviceInfo", L"DeviceInfoHolder"};
  return prop;
}

DeviceInfoHolder::DeviceInfoHolder() {
  updateDeviceInfo();
}

void DeviceInfoHolder::InitDeviceInfoHolder(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &propertyBag) noexcept {
  if (xaml::TryGetCurrentApplication()) {
    auto deviceInfoHolder = std::make_shared<DeviceInfoHolder>();
    deviceInfoHolder->updateDeviceInfo();

    propertyBag.Set(DeviceInfoHolderPropertyId(), std::move(deviceInfoHolder));

    auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

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
      // TODO: WinUI 3 Islands - set up a listener for window size changed
    }

    deviceInfoHolder->m_dpiChangedRevoker = displayInfo.DpiChanged(
        winrt::auto_revoke, [weakHolder = std::weak_ptr(deviceInfoHolder)](const auto &, const auto &) {
          if (auto strongHolder = weakHolder.lock()) {
            strongHolder->updateDeviceInfo();
          }
        });
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
  } else {
    /// TODO: WinUI 3 Island - mock for now
    m_windowWidth = 600;
    m_windowHeight = 800;
  }
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;
  m_textScaleFactor = uiSettings.TextScaleFactor();
  auto const displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  m_scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
  m_dpi = displayInfo.LogicalDpi();
  m_screenWidth = displayInfo.ScreenWidthInRawPixels();
  m_screenHeight = displayInfo.ScreenHeightInRawPixels();
  notifyChanged();
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
