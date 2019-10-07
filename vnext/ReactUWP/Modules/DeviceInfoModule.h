// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
<<<<<<< HEAD
=======

#include <IReactInstance.h>
>>>>>>> 1933859e1... some more adjustments
#include <cxxReact/Instance.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <memory>
#include <vector>

namespace react {
namespace uwp {

class DeviceInfoModule;

// TODO: Emit event to react when dimensions change.
class DeviceInfo {
 public:
  DeviceInfo(const std::shared_ptr<IReactInstance> &reactInstance);
  DeviceInfo();
  folly::dynamic GetDimensionsConstants() {
    return m_dimensions;
  }
  void update();
  void updateRootElementSize(float width, float height);
  void attachRoot(const winrt::Windows::UI::Xaml::FrameworkElement rootElement);
  void detachRoot();

 private:
  void fireEvent();
  folly::dynamic getDimensions(
      winrt::Windows::Graphics::Display::DisplayInformation displayInfo,
      winrt::Windows::UI::Core::CoreWindow window);

  folly::dynamic m_dimensions;
  winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> m_rootElement{};
  winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker
      m_sizeChangedRevoker;
  std::weak_ptr<IReactInstance> m_wkReactInstance;

  folly::dynamic getDimensions();
  void update();

 private:
  folly::dynamic getFollyDimensions(float windowWidth, float windowHeight, uint32_t screenWidth, uint32_t screenHeight, int scale, double fontScale, float dpi);
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  folly::dynamic m_dimensions;

  winrt::Windows::UI::Xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
};

class DeviceInfoModule : public facebook::xplat::module::CxxModule,
                         std::enable_shared_from_this<DeviceInfoModule> {
 public:
  DeviceInfoModule(std::shared_ptr<DeviceInfo> deviceInfo);

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char *name;

 private:
  std::shared_ptr<DeviceInfo> m_deviceInfo;
};

} // namespace uwp
} // namespace react
