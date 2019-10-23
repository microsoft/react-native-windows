// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <memory>
#include <vector>

namespace facebook {
namespace react {
class MessageQueueThread;
}
} // namespace facebook

namespace react {
namespace uwp {

class LocationObserverModule : public facebook::xplat::module::CxxModule {
 public:
  LocationObserverModule(const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread);
  virtual ~LocationObserverModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char *name;

 private:
  class LocationObserver;
  std::shared_ptr<LocationObserver> m_locationObserver;
};

} // namespace uwp
} // namespace react
