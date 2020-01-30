// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Xaml.h>
#include <memory>
#include <vector>

namespace react {
namespace uwp {

// TODO: Emit event to react when dimensions change.
class DeviceInfo {
 public:
  DeviceInfo(const std::shared_ptr<IReactInstance> &reactInstance);

  folly::dynamic GetDimensionsConstants() {
    return m_dimensions;
  }
  void update();
  void listenToUpdates();

 private:
  void fireEvent();
  folly::dynamic m_dimensions;
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  winrt::Windows::UI::Core::CoreWindow::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker m_dpiChangedRevoker{};
};

class DeviceInfoModule : public facebook::xplat::module::CxxModule {
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
