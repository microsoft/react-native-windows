// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestReactPackageProvider.h"

#include "Modules/TestAppState.h"
#include "Modules/TestDeviceInfo.h"
#include "Modules/TestModule.h"

namespace Microsoft::React::Test {

void TestReactPackageProvider::CreatePackage(msrn::IReactPackageBuilder const &packageBuilder) noexcept {
  packageBuilder.AddTurboModule(L"DeviceInfo", winrt::Microsoft::ReactNative::MakeModuleProvider<DeviceInfo>());
  packageBuilder.AddTurboModule(L"AppState", winrt::Microsoft::ReactNative::MakeModuleProvider<AppState>());
  winrt::Microsoft::ReactNative::TryAddAttributedModule(packageBuilder, L"TestModule", true);
}

} // namespace Microsoft::React::Test
