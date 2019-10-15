// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleLibraryCppPackage.h"
#include "SampleLibraryCppPackage.g.cpp"
#include "Calculator.h"
#include "MyModule.h"

using namespace winrt::Microsoft::ReactNative::Bridge;
using namespace Microsoft::ReactNative;

namespace winrt::SampleLibraryCPP::implementation {

void SampleLibraryCppPackage::CreatePackage(
    IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder);
}

} // namespace winrt::SampleLibraryCPP::implementation
