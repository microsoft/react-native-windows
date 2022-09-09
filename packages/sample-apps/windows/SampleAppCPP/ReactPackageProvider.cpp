// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageProvider.h"

#include "DebugConsole.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::SampleAppCpp::implementation {

//===========================================================================
// ReactPackageProvider implementation
//===========================================================================

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder, true);
}

} // namespace winrt::SampleAppCpp::implementation
