// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DeviceInfoModule.h"

namespace react {
namespace uwp {

//
// DeviceInfo
//

void DeviceInfo::update() {
  // IFDEF
  // auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::
  //    GetForCurrentView();

  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  // IFDEF
  // auto const &window =
  // winrt::Windows::UI::Xaml::Window::Current().CoreWindow();

  // TODO: get parent element (not window) size

  // Set Dummy values initially to avoid dependency on UI Thread
  m_dimensions = folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object("width", 100)("height", 100)(
          "scale", static_cast<int>(100) / 100)(
          "fontScale", uiSettings.TextScaleFactor())("densityDpi", 100))(
      "screenPhysicalPixels",
      folly::dynamic::object("width", 100)("height", 100)(
          "scale", static_cast<int>(100) / 100)("fontScale", 100)(
          "densityDpi", 100));
}

//
// DeviceInfoModule
//
const char *DeviceInfoModule::name = "DeviceInfo";
DeviceInfoModule *DeviceInfoModule::s_currentInstance = nullptr;

DeviceInfoModule::DeviceInfoModule(std::shared_ptr<DeviceInfo> deviceInfo)
    : m_deviceInfo(std::move(deviceInfo)) {
  s_currentInstance = this;
}

std::string DeviceInfoModule::getName() {
  return name;
}

/*static*/ void DeviceInfoModule::uiThreadAvailable() {
  if (s_currentInstance != nullptr) {
    s_currentInstance->sendDimensionsChangedEvent();
  }
}

void DeviceInfoModule::sendDimensionsChangedEvent() {
  if (auto instance = getInstance().lock()) {
    auto args = getDimensions(
        winrt::Windows::Graphics::Display::DisplayInformation::
            GetForCurrentView(),
        winrt::Windows::UI::Xaml::Window::Current().CoreWindow());

    instance->callJSFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array("didUpdateDimensions", std::move(args)));
  }
}

folly::dynamic DeviceInfoModule::getDimensions(
    winrt::Windows::Graphics::Display::DisplayInformation displayInfo,
    winrt::Windows::UI::Core::CoreWindow window) {
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  return folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object(
          "width", window != nullptr ? window.Bounds().Width : 100)(
          "height", window != nullptr ? window.Bounds().Height : 100)(
          "scale",
          displayInfo != nullptr
              ? static_cast<int>(displayInfo.ResolutionScale()) / 100
              : 1)("fontScale", uiSettings.TextScaleFactor())(
          "densityDpi",
          displayInfo != nullptr ? displayInfo.LogicalDpi() : 100))(
      "screenPhysicalPixels",
      folly::dynamic::object(
          "width",
          displayInfo != nullptr ? displayInfo.ScreenWidthInRawPixels() : 100)(
          "height",
          displayInfo != nullptr ? displayInfo.ScreenHeightInRawPixels() : 100)(
          "scale",
          displayInfo != nullptr
              ? static_cast<int>(displayInfo.ResolutionScale()) / 100
              : 1)("fontScale", uiSettings.TextScaleFactor())(
          "densityDpi",
          displayInfo != nullptr ? displayInfo.LogicalDpi() : 100));
}

std::map<std::string, folly::dynamic> DeviceInfoModule::getConstants() {
  // Set Dummy values initially to avoid dependency on UI Thread
  std::map<std::string, folly::dynamic> constants{
      {"Dimensions", getDimensions(nullptr, nullptr)}};

  return constants;
}

auto DeviceInfoModule::getMethods() -> std::vector<Method> {
  return std::vector<Method>();
}

} // namespace uwp
} // namespace react
