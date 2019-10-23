// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageBuilder.h"

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// ReactPackageBuilder implementation
//===========================================================================

ReactPackageBuilder::ReactPackageBuilder(std::shared_ptr<NativeModulesProvider> const &modulesProvider) noexcept
    : m_modulesProvider{modulesProvider} {}

void ReactPackageBuilder::AddModule(hstring const &moduleName, ReactModuleProvider const &moduleProvider) noexcept {
  m_modulesProvider->AddModuleProvider(moduleName, moduleProvider);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
