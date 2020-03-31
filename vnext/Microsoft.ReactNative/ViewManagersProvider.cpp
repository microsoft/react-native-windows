// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewManagersProvider.h"

#include "IReactContext.h"

#include "ABIViewManager.h"

namespace winrt::Microsoft::ReactNative {

/*-------------------------------------------------------------------------------
        ViewManagersProvider::GetViewManagers
-------------------------------------------------------------------------------*/
std::vector<react::uwp::NativeViewManager> ViewManagersProvider::GetViewManagers(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    std::shared_ptr<react::uwp::IReactInstance> const &instance) {
  std::vector<react::uwp::NativeViewManager> viewManagers;

  for (auto &entry : m_viewManagerProviders) {
    auto viewManagerProvider = entry.second;

    auto viewManager = std::make_unique<ABIViewManager>(instance, reactContext, viewManagerProvider());

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

} // namespace winrt::Microsoft::ReactNative
