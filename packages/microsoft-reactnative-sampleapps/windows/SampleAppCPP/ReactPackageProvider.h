// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleAppCpp::implementation {

struct ReactPackageProvider : winrt::implements<ReactPackageProvider, IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace winrt::SampleAppCpp::implementation
