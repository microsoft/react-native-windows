// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

struct MockPackageProvider : implements<MockPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder builder) {
    OnCreatePackage(builder);
  }

  std::function<void(IReactPackageBuilder)> OnCreatePackage;
};

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (ReactPackageProviderTests) {
 public:
  TEST_METHOD(CreatePackage_IsCallable) {
    int createPackageCallCount = 0;
    auto provider = make<MockPackageProvider>();
    provider.as<MockPackageProvider>()->OnCreatePackage = [&createPackageCallCount](IReactPackageBuilder builder) {
      ++createPackageCallCount;
    };

    ReactInstanceSettings settings;
    settings.PackageProviders().Append(provider);
    TestCheckEqual(1ul, settings.PackageProviders().Size());

    IReactPackageBuilder builder = Microsoft::Internal::TestController::CreateReactPackageBuilder();
    settings.PackageProviders().GetAt(0).CreatePackage(builder);
    TestCheckEqual(1, createPackageCallCount);
  }
};

} // namespace ABITests
