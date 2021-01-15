// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>
#include <sstream>
#include <string>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

namespace {

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
    TestEventService::LogEvent("succeededSignal", true);
  }

  REACT_METHOD(OnError, L"onError")
  void OnError(std::string errorMessage) noexcept {
    // intended to keep the parameter name for debug purpose
    TestEventService::LogEvent("succeededSignal", false);
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
    TestEventService::LogEvent("promiseFunctionSignal", std::move(a));
  }

  REACT_SYNC_METHOD(SyncFunction, L"syncFunction")
  std::string SyncFunction(std::string a, int b, bool c) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << (c ? "true" : "false")).str();
    return resultString;
  }

  REACT_METHOD(SyncFunctionResult, L"syncFunctionResult")
  void SyncFunctionResult(std::string a) noexcept {
    TestEventService::LogEvent("syncFunctionSignal", std::move(a));
  }

  REACT_METHOD(Constants, L"constants")
  void Constants(std::string a, int b, std::string c, int d) noexcept {
    auto resultString = (std::stringstream() << a << ", " << b << ", " << c << ", " << d).str();
    TestEventService::LogEvent("constantsSignal", std::move(resultString));
  }

  REACT_METHOD(OneCallback, L"oneCallback")
  void OneCallback(int a, int b, const std::function<void(int)> &resolve) noexcept {
    resolve(a + b);
  }

  REACT_METHOD(OneCallbackResult, L"oneCallbackResult")
  void OneCallbackResult(int r) noexcept {
    TestEventService::LogEvent("oneCallbackSignal", r);
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
    TestEventService::LogEvent("twoCallbacksResolvedSignal", r);
  }

  REACT_METHOD(TwoCallbacksRejected, L"twoCallbacksRejected")
  void TwoCallbacksRejected(std::string r) noexcept {
    TestEventService::LogEvent("twoCallbacksResolvedSignal", std::move(r));
  }
};

struct SampleTurboModuleSpec : TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"succeeded"},
      Method<void(std::string) noexcept>{1, L"onError"},
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

} // namespace

TEST_CLASS (TurboModuleTests) {
  TEST_METHOD(ExecuteSampleTurboModule) {
    TestEventService::Initialize();

    auto reactNativeHost = TestReactNativeHostHolder(L"TurboModuleTests", [](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<SampleTurboModulePackageProvider>());
    });

    TestEventService::ObserveEvents({
        TestEvent{"promiseFunctionSignal", "something, 1, true"},
        TestEvent{"oneCallbackSignal", 3},
        TestEvent{"twoCallbacksResolvedSignal", 123},
        TestEvent{"twoCallbacksResolvedSignal", "Failed"},
        TestEvent{"syncFunctionSignal", "something, 2, false"},
        TestEvent{"constantsSignal", "constantString, 3, Hello, 10"},
        TestEvent{"succeededSignal", true},
    });
  }
};

} // namespace ReactNativeIntegrationTests
