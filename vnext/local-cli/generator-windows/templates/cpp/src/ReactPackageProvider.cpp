// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPackageProvider.h"
#include "NativeModules.h"
// clang-format off
using namespace winrt::Microsoft::ReactNative;

namespace winrt::<%=ns%>::implementation
{

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
    AddAttributedModules(packageBuilder);
}

} // namespace winrt::<%=ns%>::implementation

// clang-format on
