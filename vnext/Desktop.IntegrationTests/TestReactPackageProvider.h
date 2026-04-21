// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <unknwn.h>
#include <winrt/Microsoft.ReactNative.h>

namespace Microsoft::React::Test {

namespace msrn = winrt::Microsoft::ReactNative;

struct TestReactPackageProvider : winrt::implements<TestReactPackageProvider, msrn::IReactPackageProvider> {
  void CreatePackage(msrn::IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace Microsoft::React::Test
