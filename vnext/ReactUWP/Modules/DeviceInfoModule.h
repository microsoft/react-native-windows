// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <memory>
#include <vector>

namespace react {
namespace uwp {

// TODO: Emit event to react when dimensions change.
class DeviceInfo {
 public:
  DeviceInfo() {
    update();
  }

  folly::dynamic GetDimensionsConstants() {
    return m_dimensions;
  }
  void update();

 private:
  folly::dynamic m_dimensions;
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
