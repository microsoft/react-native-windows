// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <JSI/JsiApiContext.h>
#include <NativeModules.h>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace facebook::jsi;
using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

REACT_MODULE(TestExecuteJsiModule)
struct TestExecuteJsiModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;
    TestEventService::LogEvent("initialize", nullptr);
  }

  REACT_METHOD(TestHostFunction, L"testHostFunction")
  void TestHostFunction() noexcept {
    TestEventService::LogEvent("testHostFunction started", nullptr);

    m_reactContext.CallInvoker()->invokeAsync([](Runtime &rt) {
      Function hostGreeter = Function::createFromHostFunction(
          rt,
          PropNameID::forAscii(rt, "hostGreeter"),
          1,
          [](Runtime &rt, const Value & /*thisVal*/, const Value *args, size_t count) {
            TestCheckEqual(1u, count);
            return Value{rt, String::createFromUtf8(rt, "Hello " + args[0].getString(rt).utf8(rt))};
          });
      TestCheckEqual("Hello World", hostGreeter.call(rt, "World").getString(rt).utf8(rt));
      TestCheck(hostGreeter.getHostFunction(rt) != nullptr);

      Function hostGreater2 = hostGreeter.getFunction(rt);
      TestCheck(hostGreater2.isHostFunction(rt));
      TestCheckEqual("Hello People", hostGreater2.call(rt, "People").getString(rt).utf8(rt));
      TestCheck(hostGreater2.getHostFunction(rt) != nullptr);

      TestEventService::LogEvent("testHostFunction completed", nullptr);
    });
  }

  REACT_METHOD(TestHostObject, L"testHostObject")
  void TestHostObject() noexcept {
    TestEventService::LogEvent("testHostObject started", nullptr);
    m_reactContext.CallInvoker()->invokeAsync([](Runtime &rt) {
      class GreeterHostObject : public HostObject {
        Value get(Runtime &rt, const PropNameID &) override {
          return String::createFromAscii(rt, "Hello");
        }
        void set(Runtime &, const PropNameID &, const Value &) override {}
      };

      Object hostObjGreeter = Object::createFromHostObject(rt, std::make_shared<GreeterHostObject>());
      TestCheckEqual(
          "Hello", hostObjGreeter.getProperty(rt, PropNameID::forAscii(rt, "someProp")).getString(rt).utf8(rt));
      TestCheck(hostObjGreeter.getHostObject(rt) != nullptr);

      Object hostObjGreeter2 = Value{rt, hostObjGreeter}.getObject(rt);
      TestCheck(hostObjGreeter2.isHostObject(rt));
      TestCheckEqual(
          "Hello", hostObjGreeter2.getProperty(rt, PropNameID::forAscii(rt, "someProp")).getString(rt).utf8(rt));
      TestCheck(hostObjGreeter2.getHostObject(rt) != nullptr);

      TestEventService::LogEvent("testHostObject completed", nullptr);
    });
  }

  REACT_METHOD(TestSameJsiRuntime, L"testSameJsiRuntime")
  void TestSameJsiRuntime() noexcept {
    // Make sure that we use the same facebook::jsi::Runtime when we run second time.
    // The JSI executed synchronously here because we are in JS thread.
    TestEventService::LogEvent("testSameJsiRuntime started", nullptr);
    Runtime *jsiRuntime{};
    m_reactContext.CallInvoker()->invokeAsync([&jsiRuntime](Runtime &rt) { jsiRuntime = &rt; });
    m_reactContext.CallInvoker()->invokeAsync([&jsiRuntime](Runtime &rt) {
      TestCheckEqual(jsiRuntime, &rt);
      TestEventService::LogEvent("testSameJsiRuntime completed", nullptr);
    });
  }

  REACT_METHOD(TestExecuteJsiPromise, L"testExecuteJsiPromise")
  void TestExecuteJsiPromise() noexcept {
    // Make sure that the promise is succeeded when we call ExecuteJsi.
    TestEventService::LogEvent("testExecuteJsiPromise started", nullptr);

    ReactPromise<void> callResult(
        []() noexcept { TestEventService::LogEvent("testExecuteJsiPromise promise succeeded", nullptr); },
        [](ReactError const &error) noexcept {
          TestEventService::LogEvent("testExecuteJsiPromise promise failed", error.Message.c_str());
        });
    m_reactContext.CallInvoker()->invokeAsync(
        [callResult](Runtime &) { TestEventService::LogEvent("testExecuteJsiPromise completed", nullptr);
          callResult.Resolve();
    });
  }

 private:
  ReactContext m_reactContext;
};

struct TestPackageProvider : winrt::implements<TestPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    TryAddAttributedModule(packageBuilder, L"TestExecuteJsiModule", true);
  }
};

} // namespace

TEST_CLASS (ExecuteJsiTests) {
  TEST_METHOD(Run_JSDrivenTests) {
    {
      TestEventService::Initialize();

      winrt::event_token onDestroyed{};
      auto reactNativeHost =
          TestReactNativeHostHolder(L"ExecuteJsiTests", [&onDestroyed](ReactNativeHost const &host) noexcept {
            host.PackageProviders().Append(winrt::make<TestPackageProvider>());
            onDestroyed = host.InstanceSettings().InstanceDestroyed(
                [](winrt::Windows::Foundation::IInspectable const &, InstanceDestroyedEventArgs const &args) {
                  OnInstanceDestroyed(args.Context());
                });
          });

      TestEventService::ObserveEvents({
          TestEvent{"initialize", nullptr},
          TestEvent{"testHostFunction started", nullptr},
          TestEvent{"testHostObject started", nullptr},
          TestEvent{"testSameJsiRuntime started", nullptr},
          TestEvent{"testExecuteJsiPromise started", nullptr},
          TestEvent{"testHostFunction completed", nullptr},
          TestEvent{"testHostObject completed", nullptr},
          TestEvent{"testSameJsiRuntime completed", nullptr},
          TestEvent{"testExecuteJsiPromise completed", nullptr},
          TestEvent{"testExecuteJsiPromise promise succeeded", nullptr},
      });
    }

    TestEventService::ObserveEvents({
        TestEvent{"OnInstanceDestroyed started", nullptr},
        TestEvent{"OnInstanceDestroyed promise failed", "Promise destroyed."},
    });
  }

  static void OnInstanceDestroyed(ReactContext const &reactContext) {
    // See that Jsi failed to execute
    TestEventService::LogEvent("OnInstanceDestroyed started", nullptr);

    ReactPromise<void> callResult(
        []() noexcept { TestEventService::LogEvent("OnInstanceDestroyed promise succeeded", nullptr); },
        [](ReactError const &error) noexcept {
          TestEventService::LogEvent("OnInstanceDestroyed promise failed", error.Message.c_str());
        });
    reactContext.CallInvoker()->invokeAsync(
        [](Runtime &) { TestEventService::LogEvent("OnInstanceDestroyed completed", nullptr); });
  }
};

} // namespace ReactNativeIntegrationTests
