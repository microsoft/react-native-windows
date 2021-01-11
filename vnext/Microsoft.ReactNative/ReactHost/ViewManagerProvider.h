// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <vector>

namespace react::uwp {
struct IReactInstance;
} // namespace react::uwp

namespace Microsoft::ReactNative {
class IViewManager;

using NativeViewManager = std::unique_ptr<Microsoft::ReactNative::IViewManager>;

struct ViewManagerProvider {
  virtual std::vector<NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) = 0;
};

} // namespace Microsoft::ReactNative
