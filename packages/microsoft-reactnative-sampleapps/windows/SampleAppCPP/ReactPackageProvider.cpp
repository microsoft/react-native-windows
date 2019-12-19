// Copyright (c) Microsoft Corporation. All rights reserved.
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
  AddAttributedModules(packageBuilder);
}

} // namespace winrt::SampleAppCpp::implementation
