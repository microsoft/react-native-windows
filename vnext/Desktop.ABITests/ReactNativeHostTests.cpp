// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "ActivationFactory.h"
#include "MockReactPackageProvider.h"

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

  TEST_METHOD(PackageProviders_AsConstructed_IsEmpty) {
    ReactNativeHost host{};
    Assert::AreEqual(0u, host.PackageProviders().Size());
  }

  TEST_METHOD(PackageProviders_Append_ReflectsAddition) {
    ReactNativeHost host{};
    IReactPackageProvider packageProvider = ::winrt::make<MockReactPackageProvider>();
    host.PackageProviders().Append(packageProvider);
    Assert::AreEqual(1u, host.PackageProviders().Size());
  }

  TEST_METHOD(InstanceSettings_BundleRootPathAsConstructed_IsEmpty) {
    ReactNativeHost host{};
    Assert::IsTrue(host.InstanceSettings().BundleRootPath().empty());
  }

  TEST_METHOD(InstanceSettings_BundleRootPathAsAssigned_MatchesAssignedValue) {
    const wchar_t *path = L"a/b/c";
    ReactNativeHost host{};
    host.InstanceSettings().BundleRootPath(path);
    Assert::AreEqual(std::wstring_view{path}, (std::wstring_view)host.InstanceSettings().BundleRootPath());
  }
};

} // namespace ABITests
