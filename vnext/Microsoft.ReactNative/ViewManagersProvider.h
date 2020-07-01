#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Shared/ViewManager.h>
#include "ReactHost/React.h"

#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

class ViewManagersProvider final : public Mso::React::ViewManagerProvider2 {
  std::vector<react::uwp::NativeViewManager> GetViewManagers(
      Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
      std::shared_ptr<react::uwp::IReactInstance> const &instance) override;

 public:
  ViewManagersProvider() noexcept;
  void AddViewManagerProvider(
      winrt::hstring const &viewManagerName,
      ReactViewManagerProvider const &viewManagerProvider) noexcept;

 private:
  std::unordered_map<std::string, ReactViewManagerProvider> m_viewManagerProviders;
};
} // namespace winrt::Microsoft::ReactNative
