// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"

using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::SampleApp::implementation {

struct ReactPackageProvider
    : winrt::implements<ReactPackageProvider, IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace winrt::SampleApp::implementation
