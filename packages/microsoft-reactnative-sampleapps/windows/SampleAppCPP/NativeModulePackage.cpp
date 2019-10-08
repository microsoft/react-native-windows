// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulePackage.h"
#include "DebugConsole.h"
#include "MyModule.h"
#include "MyCtorModule.h"

namespace winrt::SampleApp::implementation {

using namespace ::Microsoft::ReactNative;

//===========================================================================
// NativeModulePackage implementation
//===========================================================================

void NativeModulePackage::CreatePackage(
    Microsoft::ReactNative::Bridge::IReactPackageBuilder const&
        packageBuilder) noexcept {
  packageBuilder.AddModule(
      L"MyModule", MakeModuleProvider<TestApp::MyModule>());
  packageBuilder.AddModule(
      L"MyCtorModule", MakeModuleProvider<TestApp::MyCtorModule>());
  packageBuilder.AddModule(
      L"DebugConsole", MakeModuleProvider<TestApp::DebugConsole>());
}

} // namespace winrt::SampleApp::implementation
