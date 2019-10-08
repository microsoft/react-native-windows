// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::SampleApp::implementation {

struct NativeModulePackage : winrt::implements<NativeModulePackage, Microsoft::ReactNative::Bridge::IReactPackageProvider>
{
  NativeModulePackage() = default;

public: // IReactPackageProvider
  void CreatePackage(Microsoft::ReactNative::Bridge::IReactPackageBuilder const& packageBuilder) noexcept;
};

} // namespace winrt::CppSampleApp::implementation
