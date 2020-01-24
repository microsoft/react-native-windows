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
}

void DeviceInfo::update() {
  auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
  m_scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  auto const &window = winrt::Windows::UI::Xaml::Window::Current().CoreWindow();
  m_width = window.Bounds().Width;
  m_height = window.Bounds().Height;
  m_dimensions = folly::dynamic::object(
      "windowPhysicalPixels",
      folly::dynamic::object("width", m_width * m_scale)("height", m_height * m_scale)(
          "scale", m_scale)("fontScale", uiSettings.TextScaleFactor())("densityDpi", displayInfo.LogicalDpi()))(
      "screenPhysicalPixels",
      folly::dynamic::object("width", displayInfo.ScreenWidthInRawPixels())(
          "height", displayInfo.ScreenHeightInRawPixels())("scale", m_scale)("fontScale", uiSettings.TextScaleFactor())(
          "densityDpi", displayInfo.LogicalDpi()));

    m_dpiChangedRevoker = displayInfo.DpiChanged(winrt::auto_revoke, [this](const auto &, const auto &) {
        auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
        m_scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
        m_dimensions["windowPhysicalPixels"]["scale"] = m_scale;
        m_dimensions["windowPhysicalPixels"]["densityDpi"] = displayInfo.LogicalDpi();
        m_dimensions["screenPhysicalPixels"]["scale"] = m_scale;
        m_dimensions["screenPhysicalPixels"]["densityDpi"] = displayInfo.LogicalDpi();
        updateRootElementSize(m_width, m_height);
  });
}

void DeviceInfo::updateRootElementSize(float width, float height) {
  m_dimensions["windowPhysicalPixels"]["width"] = width * m_scale;
  m_dimensions["windowPhysicalPixels"]["height"] = height * m_scale;
  fireEvent();
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

void DeviceInfo::attachRoot(winrt::FrameworkElement rootElement) {
  m_rootElement = winrt::make_weak(rootElement);
  m_sizeChangedRevoker = rootElement.SizeChanged(winrt::auto_revoke, [this](auto &&, auto &&) {
    if (const auto root = m_rootElement.get()) {
      updateRootElementSize(static_cast<float>(root.ActualWidth()), static_cast<float>(root.ActualHeight()));
    }
  });
}

void DeviceInfo::detachRoot() {
  m_sizeChangedRevoker = {};
  m_rootElement = {};
  m_dpiChangedRevoker = {};
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
