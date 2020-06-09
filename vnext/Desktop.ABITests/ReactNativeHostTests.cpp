// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "MockReactPackageProvider.h"

using namespace winrt::Microsoft::ReactNative;

namespace ABITests {

TEST_CLASS (ReactNativeHostTests) {
 public:
  TEST_METHOD(Activation_Succeeds) {
    TestCheckNoThrow(ReactNativeHost{});
  }

  TEST_METHOD(PackageProviders_AsConstructed_IsEmpty) {
    ReactNativeHost host{};
    TestCheckEqual(0u, host.PackageProviders().Size());
  }

  TEST_METHOD(PackageProviders_Append_ReflectsAddition) {
    ReactNativeHost host{};
    IReactPackageProvider packageProvider = ::winrt::make<MockReactPackageProvider>();
    host.PackageProviders().Append(packageProvider);
    TestCheckEqual(1u, host.PackageProviders().Size());
  }

  TEST_METHOD(InstanceSettings_BundleRootPathAsConstructed_IsEmpty) {
    ReactNativeHost host{};
    TestCheck(host.InstanceSettings().BundleRootPath().empty());
  }

  TEST_METHOD(InstanceSettings_BundleRootPathAsAssigned_MatchesAssignedValue) {
    const wchar_t *path = L"a/b/c";
    ReactNativeHost host{};
    host.InstanceSettings().BundleRootPath(path);
    TestCheckEqual(std::wstring_view{path}, (std::wstring_view)host.InstanceSettings().BundleRootPath());
  }
};

} // namespace ABITests
