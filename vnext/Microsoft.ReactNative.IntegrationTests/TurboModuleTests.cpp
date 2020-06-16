// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>
#include <sstream>
#include <string>

using namespace React;
using namespace winrt::Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

REACT_MODULE(SampleTurboModule)
struct SampleTurboModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const & /*reactContext*/) noexcept {}

  REACT_CONSTANT(constantString)
  const std::string constantString{"constantString"};

  REACT_CONSTANT(constantInt)
  const int constantInt{3};

  REACT_METHOD(succeeded)
  void succeeded() noexcept {
    succeededSignal.set_value(true);
  }

  REACT_METHOD(onError)
  void onError(std::string errorMessage) noexcept {
    // intended to keep the parameter name for debug purpose
    succeededSignal.set_value(false);
  }

  REACT_METHOD(promiseFunction)
  void promiseFunction(std::string a, int b, bool c, ReactPromise<React::JSValue> result) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << (c ? "true" : "false")).str();
    result.Resolve({resultString});
    // TODO:
    // calling ".then" in the bundle fails, figure out why
    // it could be an issue about environment setup
    // since the issue doesn't happen in Playground.sln
    promiseFunctionResult(resultString);
  }

  REACT_METHOD(promiseFunctionResult)
  void promiseFunctionResult(std::string a) noexcept {
    promiseFunctionSignal.set_value(a);
  }

  REACT_SYNC_METHOD(syncFunction)
  std::string syncFunction(std::string a, int b, bool c) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << (c ? "true" : "false")).str();
    return resultString;
  }

  REACT_METHOD(syncFunctionResult)
  void syncFunctionResult(std::string a) noexcept {
    syncFunctionSignal.set_value(a);
  }

  REACT_METHOD(constants)
  void constants(std::string a, int b) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b).str();
    constantsSignal.set_value(resultString);
  }

  static std::promise<bool> succeededSignal;
  static std::promise<std::string> promiseFunctionSignal;
  static std::promise<std::string> syncFunctionSignal;
  static std::promise<std::string> constantsSignal;
};

std::promise<bool> SampleTurboModule::succeededSignal;
std::promise<std::string> SampleTurboModule::promiseFunctionSignal;
std::promise<std::string> SampleTurboModule::syncFunctionSignal;
std::promise<std::string> SampleTurboModule::constantsSignal;

struct SampleTurboModuleSpec : TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"succeeded"},
      Method<void(std::string) noexcept>{0, L"onError"},
      Method<void(std::string, int, bool, ReactPromise<React::JSValue>) noexcept>{2, L"promiseFunction"},
      Method<void(std::string) noexcept>{3, L"promiseFunctionResult"},
      SyncMethod<std::string(std::string, int, bool) noexcept>{4, L"syncFunction"},
      Method<void(std::string) noexcept>{5, L"syncFunctionResult"},
      Method<void(std::string, int) noexcept>{6, L"constants"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SampleTurboModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(0, "succeeded", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(1, "onError", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(2, "promiseFunction", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(3, "promiseFunctionResult", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(4, "syncFunction", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(5, "syncFunctionResult", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(6, "constants", "I don't care error message");
  }
};

struct SampleTurboModulePackageProvider : winrt::implements<SampleTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    auto experimental = packageBuilder.as<IReactPackageBuilderExperimental>();
    experimental.AddTurboModule(
        L"SampleTurboModule", MakeTurboModuleProvider<SampleTurboModule, SampleTurboModuleSpec>());
  }
};

TEST_CLASS (TurboModuleTests) {
  TEST_METHOD(ExecuteSampleTurboModule) {
    ReactNativeHost host{};

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

    TestCheckEqual(true, SampleTurboModule::succeededSignal.get_future().get());
    TestCheckEqual("something, 1, true", SampleTurboModule::promiseFunctionSignal.get_future().get());
    TestCheckEqual("something, 2, false", SampleTurboModule::syncFunctionSignal.get_future().get());
    TestCheckEqual("constantString, 3", SampleTurboModule::constantsSignal.get_future().get());

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
