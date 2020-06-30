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

  REACT_CONSTANT(m_constantString, L"constantString")
  const std::string m_constantString{"constantString"};

  REACT_CONSTANT(m_constantInt, L"constantInt")
  const int m_constantInt{3};

  REACT_CONSTANT_PROVIDER(GetConstants)
  void GetConstants(React::ReactConstantProvider &provider) noexcept {
    provider.Add(L"constantString2", L"Hello");
    provider.Add(L"constantInt2", 10);
  }

  REACT_METHOD(Succeeded, L"succeeded")
  void Succeeded() noexcept {
    succeededSignal.set_value(true);
  }

  REACT_METHOD(OnError, L"onError")
  void OnError(std::string errorMessage) noexcept {
    // intended to keep the parameter name for debug purpose
    succeededSignal.set_value(false);
  }

  REACT_METHOD(PromiseFunction, L"promiseFunction")
  void PromiseFunction(std::string a, int b, bool c, ReactPromise<React::JSValue> result) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << (c ? "true" : "false")).str();
    result.Resolve({resultString});
    // TODO:
    // calling ".then" in the bundle fails, figure out why
    // it could be an issue about environment setup
    // since the issue doesn't happen in Playground.sln
    PromiseFunctionResult(resultString);
  }

  REACT_METHOD(PromiseFunctionResult, L"promiseFunctionResult")
  void PromiseFunctionResult(std::string a) noexcept {
    promiseFunctionSignal.set_value(a);
  }

  REACT_SYNC_METHOD(SyncFunction, L"syncFunction")
  std::string SyncFunction(std::string a, int b, bool c) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << (c ? "true" : "false")).str();
    return resultString;
  }

  REACT_METHOD(SyncFunctionResult, L"syncFunctionResult")
  void SyncFunctionResult(std::string a) noexcept {
    syncFunctionSignal.set_value(a);
  }

  REACT_METHOD(Constants, L"constants")
  void Constants(std::string a, int b, std::string c, int d) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << c << ", " << d).str();
    constantsSignal.set_value(resultString);
  }

  REACT_METHOD(OneCallback, L"oneCallback")
  void OneCallback(int a, int b, const std::function<void(int)> &resolve) noexcept {
    resolve(a + b);
  }

  REACT_METHOD(OneCallbackResult, L"oneCallbackResult")
  void OneCallbackResult(int r) noexcept {
    oneCallbackSignal.set_value(r);
  }

  REACT_METHOD(TwoCallbacks, L"twoCallbacks")
  void TwoCallbacks(
      bool succeeded,
      int a,
      std::string b,
      const std::function<void(int)> &resolve,
      const std::function<void(std::string)> &reject) noexcept {
    if (succeeded) {
      resolve(a);
    } else {
      reject(b);
    }
  }

  REACT_METHOD(TwoCallbacksResolved, L"twoCallbacksResolved")
  void TwoCallbacksResolved(int r) noexcept {
    twoCallbacksResolvedSignal.set_value(r);
  }

  REACT_METHOD(TwoCallbacksRejected, L"twoCallbacksRejected")
  void TwoCallbacksRejected(std::string r) noexcept {
    twoCallbacksRejectedSignal.set_value(r);
  }

  static std::promise<bool> succeededSignal;
  static std::promise<std::string> promiseFunctionSignal;
  static std::promise<std::string> syncFunctionSignal;
  static std::promise<std::string> constantsSignal;
  static std::promise<int> oneCallbackSignal;
  static std::promise<int> twoCallbacksResolvedSignal;
  static std::promise<std::string> twoCallbacksRejectedSignal;
};

std::promise<bool> SampleTurboModule::succeededSignal;
std::promise<std::string> SampleTurboModule::promiseFunctionSignal;
std::promise<std::string> SampleTurboModule::syncFunctionSignal;
std::promise<std::string> SampleTurboModule::constantsSignal;
std::promise<int> SampleTurboModule::oneCallbackSignal;
std::promise<int> SampleTurboModule::twoCallbacksResolvedSignal;
std::promise<std::string> SampleTurboModule::twoCallbacksRejectedSignal;

struct SampleTurboModuleSpec : TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"succeeded"},
      Method<void(std::string) noexcept>{0, L"onError"},
      Method<void(std::string, int, bool, ReactPromise<React::JSValue>) noexcept>{2, L"promiseFunction"},
      Method<void(std::string) noexcept>{3, L"promiseFunctionResult"},
      SyncMethod<std::string(std::string, int, bool) noexcept>{4, L"syncFunction"},
      Method<void(std::string) noexcept>{5, L"syncFunctionResult"},
      Method<void(std::string, int, std::string, int) noexcept>{6, L"constants"},
      Method<void(int, int, const std::function<void(int)> &) noexcept>{7, L"oneCallback"},
      Method<void(int) noexcept>{8, L"oneCallbackResult"},
      Method<void(
          bool,
          int,
          std::string,
          const std::function<void(int)> &,
          const std::function<void(std::string)> &) noexcept>{9, L"twoCallbacks"},
      Method<void(int) noexcept>{10, L"twoCallbacksResolved"},
      Method<void(std::string) noexcept>{11, L"twoCallbacksRejected"},
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
    REACT_SHOW_METHOD_SPEC_ERRORS(7, "oneCallback", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(8, "oneCallbackResult", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(9, "twoCallbacks", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(10, "twoCallbacksResolved", "I don't care error message");
    REACT_SHOW_METHOD_SPEC_ERRORS(11, "twoCallbacksRejected", "I don't care error message");
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
    TestCheckEqual("constantString, 3, Hello, 10", SampleTurboModule::constantsSignal.get_future().get());
    TestCheckEqual(3, SampleTurboModule::oneCallbackSignal.get_future().get());
    TestCheckEqual(123, SampleTurboModule::twoCallbacksResolvedSignal.get_future().get());
    TestCheckEqual("Failed", SampleTurboModule::twoCallbacksRejectedSignal.get_future().get());

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
