// Copyright (c) Microsoft Corporation.
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

  REACT_FUNCTION(addValues, L"addValues", L"TestHostModuleFunctions")
  std::function<void(int a, int b)> addValues;

  REACT_METHOD(Start, L"start")
  void Start() noexcept {
    // Native modules are created on-demand.
    // This method is used to start loading the module from JavaScript.
  }

  REACT_METHOD(ReturnInt, L"returnInt")
  void ReturnInt(int value) noexcept {
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

  SKIPTESTMETHOD(JsFunctionCall_Succeeds) {
    std::future<TestHostModule &> testHostModule = TestHostModule::Instance.get_future();
    std::future<int> returnValue = TestHostModule::IntReturnValue.get_future();

    winrt::Microsoft::ReactNative::ReactNativeHost host{};

    auto queueController = winrt::Windows::System::DispatcherQueueController::CreateOnDedicatedThread();
    queueController.DispatcherQueue().TryEnqueue([&]() noexcept {
      host.PackageProviders().Append(winrt::make<TestPackageProvider>());

      // bundle is assumed to be co-located with the test binary
      wchar_t testBinaryPath[MAX_PATH];
      TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
      testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

      host.InstanceSettings().BundleRootPath(testBinaryPath);
      host.InstanceSettings().JavaScriptBundleFile(L"AddValues");
      host.InstanceSettings().UseDeveloperSupport(false);
      host.InstanceSettings().UseWebDebugger(false);
      host.InstanceSettings().UseFastRefresh(false);
      host.InstanceSettings().UseLiveReload(false);
      host.InstanceSettings().EnableDeveloperMenu(false);

      host.LoadInstance();
    });

    testHostModule.get().addValues(12, 23);
    TestCheckEqual(35, returnValue.get());

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
