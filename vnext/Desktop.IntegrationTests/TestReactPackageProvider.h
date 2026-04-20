// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>

namespace msrn = winrt::Microsoft::ReactNative;

struct TestReactPackageProvider : winrt::implements<TestReactPackageProvider, msrn::IReactPackageProvider> {
  void CreatePackage(msrn::IReactPackageBuilder const &packageBuilder) noexcept;
};
