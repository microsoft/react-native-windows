// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>

using namespace React;

namespace ReactNativeIntegrationTests {

REACT_MODULE(TestHostModule)
struct TestHostModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const & /*reactContext*/) noexcept {
    TestHostModule::Instance.set_value(*this);
  }

  REACT_FUNCTION(addValues)
  std::function<void(int a, int b)> addValues;

  REACT_METHOD(returnInt)
  void returnInt(int value) noexcept {
    TestHostModule::IntReturnValue.set_value(value);
  }

  static std::promise<TestHostModule &> Instance;
  static std::promise<int> IntReturnValue;
};

std::promise<TestHostModule &> TestHostModule::Instance;
std::promise<int> TestHostModule::IntReturnValue;

struct TestPackageProvider : winrt::implements<TestPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    TryAddAttributedModule(packageBuilder, L"TestHostModule");
  }
};

TEST_CLASS (ReactNativeHostTests) {
  TEST_METHOD(Activation_Succeeds) {
    TestCheckNoThrow(winrt::Microsoft::ReactNative::ReactNativeHost{});
  }

  // The test method below is being skipped as the instance creation currently crashes with the following dependency on
  // the Xaml application framework:
  //   0b VCRUNTIME140D!_CxxThrowException
  //   0c Microsoft_ReactNative!winrt::throw_hresult
  //   0d Microsoft_ReactNative!winrt::check_hresult
  //   0e
  //   Microsoft_ReactNative!winrt::impl::consume_Windows_ApplicationModel_Core_ICoreImmersiveApplication<winrt::Windows::ApplicationModel::Core::ICoreImmersiveApplication>::MainView
  //   0f Microsoft_ReactNative!<lambda_784b37f6ced458740b7689c626bf1066>::operator()
  //   10 Microsoft_ReactNative!<lambda_784b37f6ced458740b7689c626bf1066>::<lambda_invoker_cdecl>
  //   11
  //   Microsoft_ReactNative!winrt::impl::factory_cache_entry<winrt::Windows::ApplicationModel::Core::CoreApplication,winrt::Windows::ApplicationModel::Core::ICoreImmersiveApplication>::call<winrt::Windows::ApplicationModel::Core::CoreApplicationView
  //   (__cdecl*)(winrt::Windows::ApplicationModel::Core::ICoreImmersiveApplication const &)> 12
  //   Microsoft_ReactNative!winrt::impl::call_factory_cast<winrt::Windows::ApplicationModel::Core::CoreApplicationView
  //   (__cdecl*)(winrt::Windows::ApplicationModel::Core::ICoreImmersiveApplication const
  //   &),winrt::Windows::ApplicationModel::Core::CoreApplication,winrt::Windows::ApplicationModel::Core::ICoreImmersiveApplication,<lambda_784b37f6ced458740b7689c626bf1066>
  //   > 13 Microsoft_ReactNative!winrt::Windows::ApplicationModel::Core::CoreApplication::MainView 14
  //   Microsoft_ReactNative!Mso::DispatchQueueStatic::MakeMainUIScheduler 15
  //   Microsoft_ReactNative!Mso::DispatchQueueStatic::MainUIQueue 16
  //   Microsoft_ReactNative!Mso::DispatchQueue::MainUIQueue 17
  //   Microsoft_ReactNative!Mso::React::ReactInstanceWin::InitUIMessageThread 18
  //   Microsoft_ReactNative!Mso::React::ReactInstanceWin::Initialize
  // In the short term, there are several ways in which this could get mitigated (e.g. transforming the integration test
  // app into a UWP app). In the long term, the issue should get addressed by splitting the Microsoft.ReactNative.dll
  // into a "core" and a "UI" DLL, and by having the tests in this project target the "core" DLL.
  SKIPTESTMETHOD(JsFunctionCall_Succeeds) {
    std::future<TestHostModule &> testHostModule = TestHostModule::Instance.get_future();
    std::future<int> returnValue = TestHostModule::IntReturnValue.get_future();

    winrt::Microsoft::ReactNative::ReactNativeHost host{};
    host.PackageProviders().Append(winrt::make<TestPackageProvider>());

    // bundle is assumed to be co-located with the test binary
    wchar_t testBinaryPath[MAX_PATH];
    TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
    testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;
    host.InstanceSettings().BundleRootPath(testBinaryPath);

    host.InstanceSettings().JavaScriptBundleFile(L"AddValues.bundle");
    host.ReloadInstance();

    testHostModule.get().addValues(12, 23);
    TestCheckEqual(35, returnValue.get());
  }
};

} // namespace ReactNativeIntegrationTests
