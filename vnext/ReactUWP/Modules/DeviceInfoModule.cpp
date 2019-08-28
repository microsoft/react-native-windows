// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DeviceInfoModule.h"

namespace react {
namespace uwp {

//
// DeviceInfoModule
//
const char *DeviceInfoModule::name = "DeviceInfo";
DeviceInfoModule *DeviceInfoModule::s_currentInstance = nullptr;

DeviceInfoModule::DeviceInfoModule() {
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
