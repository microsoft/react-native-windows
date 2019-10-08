// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleLibraryCppPackage.h"
#include "SampleLibraryCppPackage.g.cpp"
#include "Calculator.h"
#include "MyCtorModule.h"
#include "MyModule.h"

using namespace winrt::Microsoft::ReactNative::Bridge;
using namespace Microsoft::ReactNative;

namespace winrt::SampleLibraryCPP::implementation {

void SampleLibraryCppPackage::CreatePackage(
    IReactPackageBuilder const &packageBuilder) noexcept {
  packageBuilder.AddModule(
      L"Calculator", MakeModuleProvider<::SampleLibraryCPP::Calculator>());
  packageBuilder.AddModule(
      L"MyModule", MakeModuleProvider<TestApp::MyModule>());
  packageBuilder.AddModule(
      L"MyCtorModule", MakeModuleProvider<TestApp::MyCtorModule>());
}

} // namespace winrt::SampleLibraryCPP::implementation
