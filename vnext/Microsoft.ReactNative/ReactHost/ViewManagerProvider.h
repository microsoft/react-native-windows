// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <vector>

namespace facebook::react {
class IViewManager;
}

namespace react::uwp {
struct IReactInstance;

using NativeViewManager = std::unique_ptr<facebook::react::IViewManager>;

struct ViewManagerProvider {
  virtual std::vector<NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) = 0;
};

} // namespace react::uwp
