// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewManagersProvider.h"

namespace winrt::Microsoft::ReactNative::Bridge {
/*-------------------------------------------------------------------------------
        ViewManagersProvider::GetViewManagers
-------------------------------------------------------------------------------*/
auto ViewManagersProvider::GetViewManagers(const std::shared_ptr<react::uwp::IReactInstance> &instance)
    -> std::vector<react::uwp::NativeViewManager> {
  std::vector<react::uwp::NativeViewManager> viewManagers;

  return viewManagers;
}
} // namespace winrt::Microsoft::ReactNative::Bridge
