#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactUWP/ViewManagerProvider.h>
#include <ReactWindowsCore/ViewManager.h>

#include <winrt/Microsoft.ReactNative.Bridge.h>
#include <winrt/Microsoft.ReactNative.h>

#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::Bridge {
class ViewManagersProvider final : public react::uwp::ViewManagerProvider {
 public:
  virtual std::vector<react::uwp::NativeViewManager> GetViewManagers(
      const std::shared_ptr<react::uwp::IReactInstance> &instance) override;

 public:
  ViewManagersProvider() noexcept;
  void AddViewManagerProvider(
      winrt::hstring const &viewManagerName,
      ReactViewManagerProvider const &viewManagerProvider) noexcept;

 private:
  std::unordered_map<std::string, Microsoft::ReactNative::Bridge::ReactViewManagerProvider> m_viewManagerProviders;
};
} // namespace winrt::Microsoft::ReactNative::Bridge
