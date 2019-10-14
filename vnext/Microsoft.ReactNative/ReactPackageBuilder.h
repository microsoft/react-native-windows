#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "NativeModulesProvider.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ReactPackageBuilder
    : winrt::implements<ReactPackageBuilder, IReactPackageBuilder> {
  ReactPackageBuilder(
      std::shared_ptr<NativeModulesProvider> const &modulesProvider) noexcept;

 public: // IReactPackageBuilder
  void AddModule(
      hstring const &moduleName,
      ReactModuleProvider const &moduleProvider) noexcept;

 private:
  std::shared_ptr<NativeModulesProvider> m_modulesProvider;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
