// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewManagersProvider.h"

#include "IReactContext.h"

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

    auto reactContext = winrt::make<ReactContext>(instance).as<IReactContext>();
    auto viewManager = std::make_unique<ABIViewManager>(instance, viewManagerProvider(reactContext));

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
