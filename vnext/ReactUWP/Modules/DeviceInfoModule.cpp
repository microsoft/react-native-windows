// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

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

DeviceInfo::DeviceInfo() {
  m_dimensions = getDimensions(nullptr, nullptr);
}

folly::dynamic DeviceInfo::getDimensions(
    winrt::Windows::Graphics::Display::DisplayInformation displayInfo,
    winrt::Windows::UI::Core::CoreWindow window) {
  winrt::Windows::UI::ViewManagement::UISettings uiSettings;

  m_dimensions = folly::dynamic::object(
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

void DeviceInfo::update() {
  m_dimensions = getDimensions(
      winrt::Windows::Graphics::Display::DisplayInformation::
          GetForCurrentView(),
      winrt::Windows::UI::Xaml::Window::Current().CoreWindow());
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
    : m_deviceInfo(std::move(deviceInfo)) {
  m_leavingBackgroundRevoker =
      winrt::Windows::UI::Xaml::Application::Current().LeavingBackground(
          winrt::auto_revoke,
          [weakThis = std::weak_ptr<DeviceInfoModule>(shared_from_this())](winrt::Windows::Foundation::IInspectable const& /*sender*/,
            winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const
            & /*e*/) {
            if (auto strongThis = weakThis.lock()) {
              strongThis->sendDimensionsChangedEvent();
            }
          });
}

std::string DeviceInfoModule::getName() {
  return name;
}

void DeviceInfoModule::sendDimensionsChangedEvent() {
  if (auto instance = getInstance().lock()) {
    m_deviceInfo->update();
    auto args = m_deviceInfo->GetDimensionsConstants();

    instance->callJSFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array("didUpdateDimensions", std::move(args)));
  }
}

std::map<std::string, folly::dynamic> DeviceInfoModule::getConstants() {
  // Set Dummy values initially to avoid dependency on UI Thread
  std::map<std::string, folly::dynamic> constants{
      {"Dimensions", m_deviceInfo->GetDimensionsConstants()}};

  return constants;
}

auto DeviceInfoModule::getMethods() -> std::vector<Method> {
  return std::vector<Method>();
}

} // namespace uwp
} // namespace react
