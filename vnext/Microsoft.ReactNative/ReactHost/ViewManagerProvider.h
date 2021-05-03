// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <vector>

namespace Microsoft::ReactNative {
class IViewManager;
struct IReactInstance;

using NativeViewManager = std::unique_ptr<Microsoft::ReactNative::IViewManager>;

struct ViewManagerProvider {
  virtual std::vector<NativeViewManager> GetViewManagers(const std::shared_ptr<IReactInstance> &instance) = 0;
};

} // namespace Microsoft::ReactNative
