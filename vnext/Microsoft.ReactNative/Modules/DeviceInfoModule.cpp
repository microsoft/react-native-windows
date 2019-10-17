// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Core.h>
#include "DeviceInfoModule.h"

namespace react {
namespace uwp {

//
// DeviceInfo
//
DeviceInfo::DeviceInfo(const std::shared_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance) {
  update();
}

void DeviceInfo::update() {
  auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::
      GetForCurrentView();
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  auto window = winrt::Windows::UI::Xaml::Window::Current().CoreWindow();

  m_dimensions = folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object("width", window.Bounds().Width)(
          "height", window.Bounds().Height)(
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

void DeviceInfo::updateRootElementSize(float width, float height) {
  m_dimensions["windowPhysicalPixels"]["width"] = width;
  m_dimensions["windowPhysicalPixels"]["height"] = height;
  fireEvent();
}

void DeviceInfo::fireEvent() {
  auto instance = m_wkReactInstance.lock();
  if (instance) {
    instance->CallJsFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array(
            "didUpdateDimensions", std::move(GetDimensionsConstants())));
  }
}

void DeviceInfo::attachRoot(winrt::FrameworkElement rootElement) {
  m_rootElement = winrt::make_weak(rootElement);
  m_sizeChangedRevoker =
      rootElement.SizeChanged(winrt::auto_revoke, [this](auto &&, auto &&) {
        if (const auto root = m_rootElement.get()) {
          updateRootElementSize(
              static_cast<float>(root.ActualWidth()),
              static_cast<float>(root.ActualHeight()));
        }
      });
}

void DeviceInfo::detachRoot() {
  m_sizeChangedRevoker = {};
  m_rootElement = {};
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
