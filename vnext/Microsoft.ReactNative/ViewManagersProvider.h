#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactSupport.h"
#include <ReactUWP/ViewManagerProvider.h>
#include <ReactWindowsCore/ViewManager.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative::Bridge {
class ViewManagersProvider final : public react::uwp::ViewManagerProvider {
 public:
  virtual std::vector<react::uwp::NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) override;
};
} // namespace Microsoft::ReactNative::Bridge
