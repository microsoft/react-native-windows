// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewManagersProvider.h"

#include "ABIViewManager.h"

namespace winrt::Microsoft::ReactNative::Bridge {
/*-------------------------------------------------------------------------------
        ViewManagersProvider::GetViewManagers
-------------------------------------------------------------------------------*/
std::vector<react::uwp::NativeViewManager> ViewManagersProvider::GetViewManagers(
    const std::shared_ptr<react::uwp::IReactInstance> &instance) {
  std::vector<react::uwp::NativeViewManager> viewManagers;

  for (auto &entry : m_viewManagerProviders) {
    auto viewManagerProvider = entry.second;

    auto viewManager = std::make_unique<ABIViewManager>(instance, viewManagerProvider());

    viewManagers.emplace_back(std::move(viewManager));
  }

  return viewManagers;
}

ViewManagersProvider::ViewManagersProvider() noexcept {}

void ViewManagersProvider::AddViewManagerProvider(
    winrt::hstring const &viewManagerName,
    ReactViewManagerProvider const &viewManagerProvider) noexcept {
  m_viewManagerProviders.emplace(to_string(viewManagerName), viewManagerProvider);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
