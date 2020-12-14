// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <JSI/JsiApi.h>
#include <NativeModules.h>
#include <winrt/Windows.System.h>
#include "MockReactPackageProvider.h"

using namespace React;

namespace ReactNativeIntegrationTests {

REACT_MODULE(TestHostModule)
struct TestHostModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    using namespace facebook::jsi;
    TestHostModule::Instance.set_value(*this);

    Runtime *jsiRuntime{nullptr};
    bool jsiExecuted{false};
    ExecuteJsi(reactContext, [&](Runtime &rt) {
      jsiRuntime = &rt;
      jsiExecuted = true;
      auto eval = rt.global().getPropertyAsFunction(rt, "eval");
      auto addFunc = eval.call(rt, "(function(x, y) { return x + y; })").getObject(rt).getFunction(rt);
      TestCheckEqual(7, addFunc.call(rt, 3, 4).getNumber());

      Function hostGreeter = Function::createFromHostFunction(
          rt,
          PropNameID::forAscii(rt, "multFunc"),
          1,
          [](Runtime &rt, const Value & /*thisVal*/, const Value *args, size_t /*count*/) {
            return Value{rt, String::createFromUtf8(rt, "Hello " + args[0].getString(rt).utf8(rt))};
          });
      TestCheckEqual("Hello World", hostGreeter.call(rt, "World").getString(rt).utf8(rt));
      TestCheck(hostGreeter.getHostFunction(rt) != nullptr);

      Function hostGreater2 = hostGreeter.getFunction(rt);
      TestCheck(hostGreater2.isHostFunction(rt));
      TestCheckEqual("Hello World", hostGreater2.call(rt, "World").getString(rt).utf8(rt));
      TestCheck(hostGreater2.getHostFunction(rt) != nullptr);

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
    });
    TestCheck(jsiExecuted);

    jsiExecuted = false;
    ExecuteJsi(reactContext, [&](Runtime &rt) {
      jsiExecuted = true;
      // Make sure that our ReactContext offers the same facebook::jsi::Runtime
      TestCheckEqual(jsiRuntime, &rt);
    });
    TestCheck(jsiExecuted);
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

  TEST_METHOD(JsFunctionCall_Succeeds) {
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

      host.InstanceSettings().UseDirectDebugger(true);

      host.LoadInstance();
    });

    testHostModule.get().addValues(12, 23);
    TestCheckEqual(35, returnValue.get());

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
