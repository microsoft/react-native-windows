// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>
#include <string>

using namespace React;

namespace ReactNativeIntegrationTests {

REACT_MODULE(SampleTurboModule)
struct SampleTurboModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const & /*reactContext*/) noexcept {}

  REACT_METHOD(voidFunction)
  void voidFunction() noexcept {
    voidFunctionSignal.set_value(65536);
  }

  static std::promise<int> voidFunctionSignal;
};

std::promise<int> SampleTurboModule::voidFunctionSignal;

struct SampleTurboModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"voidFunction"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SampleTurboModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(0, "voidFunction", "I don't care error message");
  }
};

struct SampleTurboModulePackageProvider : winrt::implements<SampleTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    auto experimental = packageBuilder.as<IReactPackageBuilderExperimental>();
    experimental.AddTurboModule(
        L"SampleTurboModule",
        winrt::Microsoft::ReactNative::MakeTurboModuleProvider<SampleTurboModule, SampleTurboModuleSpec>());
  }
};

TEST_CLASS (TurboModuleTests) {
  TEST_METHOD(ExecuteSampleTurboModule) {
    winrt::Microsoft::ReactNative::ReactNativeHost host{};

    auto queueController = winrt::Windows::System::DispatcherQueueController::CreateOnDedicatedThread();
    queueController.DispatcherQueue().TryEnqueue([&]() noexcept {
      host.PackageProviders().Append(winrt::make<SampleTurboModulePackageProvider>());

      // bundle is assumed to be co-located with the test binary
      wchar_t testBinaryPath[MAX_PATH];
      TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
      testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

      host.InstanceSettings().BundleRootPath(testBinaryPath);
      host.InstanceSettings().JavaScriptBundleFile(L"TurboModuleTests");
      host.InstanceSettings().UseDeveloperSupport(false);
      host.InstanceSettings().UseWebDebugger(false);
      host.InstanceSettings().UseFastRefresh(false);
      host.InstanceSettings().UseLiveReload(false);
      host.InstanceSettings().EnableDeveloperMenu(false);

      host.LoadInstance();
    });

    TestCheckEqual(1, SampleTurboModule::voidFunctionSignal.get_future().get());

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
