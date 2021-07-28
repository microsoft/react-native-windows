// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewManagersProvider.h"

#include "IReactContext.h"

#include "ABIViewManager.h"

namespace winrt::Microsoft::ReactNative {

/*-------------------------------------------------------------------------------
        ViewManagersProvider::GetViewManagers
-------------------------------------------------------------------------------*/
std::vector<std::unique_ptr<::Microsoft::ReactNative::IViewManager>> ViewManagersProvider::GetViewManagers(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext) {
  std::vector<std::unique_ptr<::Microsoft::ReactNative::IViewManager>> viewManagers;

  for (auto &entry : m_viewManagerProviders) {
    auto viewManagerProvider = entry.second;

    auto viewManager = viewManagerProvider(reactContext);

    viewManagers.emplace_back(std::move(viewManager));
  }

  return viewManagers;
}

ViewManagersProvider::ViewManagersProvider() noexcept {}

void ViewManagersProvider::AddViewManagerProvider(
    winrt::hstring const &viewManagerName,
    ReactViewManagerProvider const &viewManagerProvider) noexcept {
  m_viewManagerProviders.emplace(
      to_string(viewManagerName), [viewManagerProvider](Mso::CntPtr<Mso::React::IReactContext> const &reactContext) {
        return std::make_unique<ABIViewManager>(reactContext, viewManagerProvider());
      });
}

void ViewManagersProvider::AddViewManagerProvider(
    winrt::hstring const &viewManagerName,
    std::function<std::unique_ptr<::Microsoft::ReactNative::IViewManager>(
        Mso::CntPtr<Mso::React::IReactContext> const &)> const &viewManagerProvider) noexcept {
  m_viewManagerProviders.emplace(to_string(viewManagerName), viewManagerProvider);
}

} // namespace winrt::Microsoft::ReactNative
