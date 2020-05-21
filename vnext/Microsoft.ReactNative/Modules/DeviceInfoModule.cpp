// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceInfoModule.h"
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace Microsoft::ReactNative {

React::JSValueObject GetDimensionsConstants() {
  auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  auto scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
  auto const &window = xaml::Window::Current().CoreWindow();
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  return React::JSValueObject{
      {"windowPhysicalPixels",
       React::JSValueObject{{"width", window.Bounds().Width * scale},
        {"height", window.Bounds().Height * scale},
        {"scale", scale},
        {"fontScale", uiSettings.TextScaleFactor()},
        {"densityDpi", displayInfo.LogicalDpi()}}},
      {"screenPhysicalPixels",
       React::JSValueObject{{"width", displayInfo.ScreenWidthInRawPixels()},
        {"height", displayInfo.ScreenHeightInRawPixels()},
        {"scale", scale},
        {"fontScale", uiSettings.TextScaleFactor()},
        {"densityDpi", displayInfo.LogicalDpi()}}},
  };
}

void DeviceInfo::getConstants(React::ReactConstantProvider &provider) noexcept {
  provider.Add(L"Dimensions", std::move(GetDimensionsConstants()));
}

void DeviceInfo::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  auto const &window = xaml::Window::Current().CoreWindow();

  m_sizeChangedRevoker = window.SizeChanged(winrt::auto_revoke, [this](auto &&, auto &&) {
    emitUpdate();
  });

  m_dpiChangedRevoker = displayInfo.DpiChanged(winrt::auto_revoke, [this](const auto &, const auto &) {
    emitUpdate();
  });
}

void DeviceInfo::emitUpdate() noexcept {
  m_context.EmitJSEvent(
      L"RCTDeviceEventEmitter", L"didUpdateDimensions", GetDimensionsConstants());
}

} // namespace Microsoft::ReactNative
