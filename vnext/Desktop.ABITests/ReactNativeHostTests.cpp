// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "ActivationFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

TEST_CLASS (ReactNativeHostTests) {
 public:
  ReactNativeHostTests() noexcept {
    winrt_activation_handler = WINRT_RoGetActivationFactory;
  }

  TEST_METHOD(Activation_Succeeds) {
    try {
      ReactNativeHost host{};
      Assert::IsTrue(true);
    } catch (...) {
      Assert::Fail();
    }
  }

  // Functions under test:
  // IVector<IReactPackageProvider> PackageProviders { get; set; };
  // ReactInstanceSettings InstanceSettings { get; set; };

  TEST_METHOD(PackageProviders_AsConstructed_IsEmpty) {
    ReactNativeHost host{};
    Assert::AreEqual(0u, host.PackageProviders().Size());
  }

  class MockPackageProvider
      : public winrt::implements<MockPackageProvider, ::winrt::Microsoft::ReactNative::IReactPackageProvider> {
   public:
    MockPackageProvider(){};
    void CreatePackage(IReactPackageBuilder const &packageBuilder){};
  };

  TEST_METHOD(PackageProviders_Append_ReflectsAddition) {
    winrt::Microsoft::ReactNative::ReactNativeHost host{};

    IReactPackageProvider packageProvider = ::winrt::make<MockPackageProvider>();

    host.PackageProviders().Append(packageProvider);
    Assert::AreEqual(1u, host.PackageProviders().Size());
  }
};

} // namespace ABITests
