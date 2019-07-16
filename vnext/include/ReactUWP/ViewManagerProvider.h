// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <vector>

namespace facebook {
namespace react {
class IViewManager;
}
} // namespace facebook
namespace react {
namespace uwp {
struct IReactInstance;
}
} // namespace react

namespace react {
namespace uwp {

using NativeViewManager = std::unique_ptr<facebook::react::IViewManager>;

struct ViewManagerProvider {
  virtual std::vector<NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) = 0;
};

} // namespace uwp
} // namespace react
