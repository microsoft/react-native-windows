// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModulePackage.h"
#include "DebugConsole.h"
#include "MyModule.h"

namespace winrt::SampleApp::implementation {

using namespace ::Microsoft::ReactNative;

//===========================================================================
// NativeModulePackage implementation
//===========================================================================

void NativeModulePackage::CreateModuleProviders(
    Microsoft::ReactNative::Bridge::ModuleProviderAdder
        addModuleProvider) noexcept {
  addModuleProvider(L"MyModule", MakeModuleProvider<TestApp::MyModule>());
  addModuleProvider(
      L"DebugConsole", MakeModuleProvider<TestApp::DebugConsole>());
}

} // namespace winrt::SampleApp::implementation
