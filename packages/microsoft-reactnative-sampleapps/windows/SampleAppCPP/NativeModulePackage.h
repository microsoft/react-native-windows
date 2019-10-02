// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::SampleApp::implementation {

struct NativeModulePackage : winrt::implements<NativeModulePackage, Microsoft::ReactNative::Bridge::INativeModulePackage>
{
  NativeModulePackage() = default;

public: // INativeModulePackage
  void CreateModuleProviders(Microsoft::ReactNative::Bridge::ModuleProviderAdder addModuleProvider) noexcept;
};

} // namespace winrt::CppSampleApp::implementation
