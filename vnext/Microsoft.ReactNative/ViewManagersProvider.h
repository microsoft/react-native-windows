#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewManager.h>
#include "ReactHost/React.h"

#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

class ViewManagersProvider final : public Mso::React::ViewManagerProvider2 {
  std::vector<std::unique_ptr<::Microsoft::ReactNative::IViewManager>> GetViewManagers(
      Mso::CntPtr<Mso::React::IReactContext> const &reactContext) override;

 public:
  ViewManagersProvider() noexcept;
  void AddViewManagerProvider(
      winrt::hstring const &viewManagerName,
      ReactViewManagerProvider const &viewManagerProvider) noexcept;
  void AddViewManagerProvider(
      winrt::hstring const &viewManagerName,
      std::function<std::unique_ptr<::Microsoft::ReactNative::IViewManager>(
          Mso::CntPtr<Mso::React::IReactContext> const &)> const &viewManagerProvider) noexcept;

 private:
  std::unordered_map<
      std::string,
      std::function<std::unique_ptr<::Microsoft::ReactNative::IViewManager>(
          Mso::CntPtr<Mso::React::IReactContext> const &)>>
      m_viewManagerProviders;
};
} // namespace winrt::Microsoft::ReactNative
