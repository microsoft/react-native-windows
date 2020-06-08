// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.h>
#include <functional>

using namespace winrt::Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

class MockReactPackageProvider : public winrt::implements<MockReactPackageProvider, IReactPackageProvider> {
 public:
  MockReactPackageProvider(){};
  void CreatePackage(IReactPackageBuilder const &packageBuilder) {
    MockCreatePackage(packageBuilder);
  };

  std::function<void(IReactPackageBuilder const &)> MockCreatePackage;
};

} // namespace ReactNativeIntegrationTests
