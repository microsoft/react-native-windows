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

  REACT_METHOD(TestSimpleExecuteJsi, L"testSimpleExecuteJsi")
  void TestSimpleExecuteJsi() noexcept {
    TestEventService::LogEvent("testSimpleExecuteJsi started", nullptr);
    ExecuteJsi(m_reactContext, [](Runtime &rt) {
      auto eval = rt.global().getPropertyAsFunction(rt, "eval");
      auto addFunc = eval.call(rt, "(function(x, y) { return x + y; })").getObject(rt).getFunction(rt);
      TestCheckEqual(7, addFunc.call(rt, 3, 4).getNumber());
      TestEventService::LogEvent("testSimpleExecuteJsi completed", nullptr);
    });
  }

  REACT_METHOD(TestHostFunction, L"testHostFunction")
  void TestHostFunction() noexcept {
    TestEventService::LogEvent("testHostFunction started", nullptr);
    ExecuteJsi(m_reactContext, [](Runtime &rt) {
      Function hostGreeter = Function::createFromHostFunction(
          rt,
          PropNameID::forAscii(rt, "hostGreeter"),
          1,
          [](Runtime &rt, const Value & /*thisVal*/, const Value *args, size_t count) {
            TestCheckEqual(1, count);
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
    ExecuteJsi(m_reactContext, [](Runtime &rt) {
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
    ExecuteJsi(m_reactContext, [&jsiRuntime](Runtime &rt) { jsiRuntime = &rt; });
    ExecuteJsi(m_reactContext, [&jsiRuntime](Runtime &rt) {
      TestCheckEqual(jsiRuntime, &rt);
      TestEventService::LogEvent("testSameJsiRuntime completed", nullptr);
    });
  }

 private:
  ReactContext m_reactContext;
};

struct TestPackageProvider : winrt::implements<TestPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    TryAddAttributedModule(packageBuilder, L"TestExecuteJsiModule");
  }
};

} // namespace

TEST_CLASS (ExecuteJsiTests) {
  TEST_METHOD(Run_JSDrivenTests) {
    TestEventService::Initialize();

    auto reactNativeHost = TestReactNativeHostHolder(L"ExecuteJsiTests", [](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<TestPackageProvider>());
    });

    TestEventService::ObserveEvents({
        TestEvent{"initialize", nullptr},
        TestEvent{"testSimpleExecuteJsi started", nullptr},
        TestEvent{"testSimpleExecuteJsi completed", nullptr},
        TestEvent{"testHostFunction started", nullptr},
        TestEvent{"testHostFunction completed", nullptr},
        TestEvent{"testHostObject started", nullptr},
        TestEvent{"testHostObject completed", nullptr},
        TestEvent{"testSameJsiRuntime started", nullptr},
        TestEvent{"testSameJsiRuntime completed", nullptr},
    });
  }
};

} // namespace ReactNativeIntegrationTests
