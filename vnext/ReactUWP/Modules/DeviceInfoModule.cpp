// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "DeviceInfoModule.h"

namespace react {
namespace uwp {

//
// DeviceInfo
//
DeviceInfo::DeviceInfo(const std::shared_ptr<IReactInstance> &reactInstance) : m_wkReactInstance(reactInstance) {
  update();
  listenToUpdates();
}

void DeviceInfo::listenToUpdates() {
  auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  auto const &window = winrt::Windows::UI::Xaml::Window::Current().CoreWindow();

  m_sizeChangedRevoker = window.SizeChanged(winrt::auto_revoke, [this](auto &&, auto &&) {
    update();
    fireEvent();
  });

  m_dpiChangedRevoker = displayInfo.DpiChanged(winrt::auto_revoke, [this](const auto &, const auto &) {
    update();
    fireEvent();
  });
}

void DeviceInfo::update() {
  auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  auto scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;

  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  auto const &window = winrt::Windows::UI::Xaml::Window::Current().CoreWindow();

  m_dimensions = folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object("width", window.Bounds().Width * scale)("height", window.Bounds().Height * scale)(
          "scale", scale)("fontScale", uiSettings.TextScaleFactor())("densityDpi", displayInfo.LogicalDpi()))(
      "screenPhysicalPixels",
      folly::dynamic::object("width", displayInfo.ScreenWidthInRawPixels())(
          "height", displayInfo.ScreenHeightInRawPixels())("scale", scale)("fontScale", uiSettings.TextScaleFactor())(
          "densityDpi", displayInfo.LogicalDpi()));
}

void DeviceInfo::fireEvent() {
  auto instance = m_wkReactInstance.lock();
  if (instance) {
    instance->CallJsFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array("didUpdateDimensions", std::move(GetDimensionsConstants())));
  }
}

//
// DeviceInfoModule
//
const char *DeviceInfoModule::name = "DeviceInfo";

DeviceInfoModule::DeviceInfoModule(std::shared_ptr<DeviceInfo> deviceInfo) : m_deviceInfo(std::move(deviceInfo)) {}

std::string DeviceInfoModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> DeviceInfoModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{{"Dimensions", m_deviceInfo->GetDimensionsConstants()}};

  return constants;
}

auto DeviceInfoModule::getMethods() -> std::vector<Method> {
  return std::vector<Method>();
}

} // namespace uwp
} // namespace react
