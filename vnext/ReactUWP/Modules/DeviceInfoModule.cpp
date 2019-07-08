// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "DeviceInfoModule.h"

namespace react {
namespace uwp {

//
// DeviceInfo
//

void DeviceInfo::update() {
  auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::
      GetForCurrentView();
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  // TODO: get window / parent element size
  m_dimensions = folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object("width", 100)("height", 100)(
          "scale", static_cast<int>(displayInfo.ResolutionScale()) / 100)(
          "fontScale", uiSettings.TextScaleFactor())(
          "densityDpi", displayInfo.LogicalDpi()))(
      "screenPhysicalPixels",
      folly::dynamic::object("width", displayInfo.ScreenWidthInRawPixels())(
          "height", displayInfo.ScreenHeightInRawPixels())(
          "scale", static_cast<int>(displayInfo.ResolutionScale()) / 100)(
          "fontScale", uiSettings.TextScaleFactor())(
          "densityDpi", displayInfo.LogicalDpi()));
}

//
// DeviceInfoModule
//
const char *DeviceInfoModule::name = "DeviceInfo";

DeviceInfoModule::DeviceInfoModule(std::shared_ptr<DeviceInfo> deviceInfo)
    : m_deviceInfo(std::move(deviceInfo)) {}

std::string DeviceInfoModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> DeviceInfoModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{
      {"Dimensions", m_deviceInfo->GetDimensionsConstants()}};

  return constants;
}

auto DeviceInfoModule::getMethods() -> std::vector<Method> {
  return std::vector<Method>();
}

} // namespace uwp
} // namespace react