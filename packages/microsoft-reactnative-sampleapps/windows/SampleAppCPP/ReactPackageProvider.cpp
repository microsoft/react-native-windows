// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageProvider.h"

#include "DebugConsole.h"

using namespace Microsoft::ReactNative;
using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::SampleApp::implementation {

//===========================================================================
// ReactPackageProvider implementation
//===========================================================================

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
  AddAttributedModules(packageBuilder);
}

} // namespace winrt::SampleApp::implementation
