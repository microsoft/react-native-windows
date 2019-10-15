// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "SampleLibraryCppPackage.g.h"

using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::SampleLibraryCPP::implementation {

struct SampleLibraryCppPackage
    : SampleLibraryCppPackageT<SampleLibraryCppPackage> {
  SampleLibraryCppPackage() = default;

  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace winrt::SampleLibraryCPP::implementation

namespace winrt::SampleLibraryCPP::factory_implementation {

struct SampleLibraryCppPackage : SampleLibraryCppPackageT<
                                     SampleLibraryCppPackage,
                                     implementation::SampleLibraryCppPackage> {
};

} // namespace winrt::SampleLibraryCPP::factory_implementation
