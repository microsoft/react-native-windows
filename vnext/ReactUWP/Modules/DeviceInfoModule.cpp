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

  // The LeavingBackgound event subscription must be set up from the UI Thread.
  // Subscribing to this event on other threads does not throw. But the handler won't be called when you expect.
  m_leavingBackgroundRevoker =
    winrt::Windows::UI::Xaml::Application::Current().LeavingBackground(
      winrt::auto_revoke,
      [wk_parent = wk_parent](
        winrt::Windows::Foundation::IInspectable const& /*sender*/,
        winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const
        & /*e*/) {
          if (auto parent = wk_parent.lock()) {
            parent->sendDimensionsChangedEvent();
          }
      });
}

folly::dynamic DeviceInfo::getDimensions() {

  try {
    auto displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
    auto const& window = winrt::Windows::UI::Xaml::Window::Current().CoreWindow();
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;

    return getFollyDimensions(window.Bounds().Width, window.Bounds().Height, displayInfo.ScreenWidthInRawPixels(), displayInfo.ScreenHeightInRawPixels(), static_cast<int>(displayInfo.ResolutionScale()) / 100, uiSettings.TextScaleFactor(), displayInfo.LogicalDpi());
  }
  catch (...) {
    // Check for HResult E_RPC_WRONG_THREAD

    // Send fake data when we are in the background.
    // Using 1 for all values since it should be obviously fake, and avoids consumers dividing by 0
    return getFollyDimensions(1, 1, 1, 1, 1, 1, 1);
  }
}


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
		  
folly::dynamic DeviceInfo::getFollyDimensions(float windowWidth, float windowHeight, uint32_t screenWidth, uint32_t screenHeight, int scale, double fontScale, float dpi) {
  
  return folly::dynamic::object(
    "windowPhysicalPixels",
    folly::dynamic::object(
      "width", windowWidth)(
      "height", windowHeight)(
      "scale", scale)(
      "fontScale", fontScale)(
      "densityDpi", dpi))(
    "screenPhysicalPixels",
    folly::dynamic::object(
      "width", screenWidth)(
      "height", screenHeight)(
      "scale", scale)(
      "fontScale", fontScale)(
      "densityDpi", dpi));
}

void DeviceInfo::setParent(std::weak_ptr<DeviceInfoModule> parent) {
  wk_parent = parent;
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

  m_deviceInfo->setParent(std::weak_ptr<DeviceInfoModule>(shared_from_this()));
}

std::string DeviceInfoModule::getName() {
  return name;
}

void DeviceInfoModule::sendDimensionsChangedEvent() {
  if (auto instance = getInstance().lock()) {
    instance->callJSFunction(
        "RCTDeviceEventEmitter",
        "emit",
        folly::dynamic::array("didUpdateDimensions", std::move(m_deviceInfo->getDimensions())));
  }
}

std::map<std::string, folly::dynamic> DeviceInfoModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{
      {"Dimensions", m_deviceInfo->getDimensions()}};

  return constants;
}

auto DeviceInfoModule::getMethods() -> std::vector<Method> {
  return std::vector<Method>();
}

} // namespace uwp
} // namespace react
