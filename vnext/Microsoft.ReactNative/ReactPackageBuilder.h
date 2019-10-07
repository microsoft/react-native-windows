#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ReactPackageBuilder
    : winrt::implements<ReactPackageBuilder, IReactPackageBuilder> {
  ReactPackageBuilder() = default;

 public: // IReactPackageBuilder
  void AddModule(
      hstring const &moduleName,
      ReactModuleProvider moduleProvider) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
