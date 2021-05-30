// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// See that we can safely reload react instance when we use C++ TurboModules.

#include "pch.h"
#include <ReactCommon/TurboModule.h>
#include <ReactCommon/TurboModuleUtils.h>
#include <TurboModuleProvider.h> // It is RNW specific
#include <limits>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace facebook;
using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

// In this test we put spec definition that normally must be generated.
// >>>> Start generated

// The spec from .h file
struct MyTrivialTurboModuleSpec : react::TurboModule {
  virtual void startFromJS(jsi::Runtime &rt) = 0;

 protected:
  MyTrivialTurboModuleSpec(std::shared_ptr<react::CallInvoker> jsInvoker);
};

// The spec from .cpp file

static jsi::Value MyTrivialTurboModuleSpec_startFromJS(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 0);
  static_cast<MyTrivialTurboModuleSpec *>(&turboModule)->startFromJS(rt);
  return jsi::Value::undefined();
}

MyTrivialTurboModuleSpec::MyTrivialTurboModuleSpec(std::shared_ptr<react::CallInvoker> jsInvoker)
    : react::TurboModule("MyTrivialTurboModuleSpec", std::move(jsInvoker)) {
  methodMap_.try_emplace("startFromJS", MethodMetadata{0, MyTrivialTurboModuleSpec_startFromJS});
}

// <<<< End generated

struct MyTrivialTurboModule : MyTrivialTurboModuleSpec {
  MyTrivialTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker);

  void startFromJS(jsi::Runtime &rt) override;
};

MyTrivialTurboModule::MyTrivialTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker)
    : MyTrivialTurboModuleSpec(std::move(jsInvoker)) {}

void MyTrivialTurboModule::startFromJS(jsi::Runtime & /*rt*/) {
  TestEventService::LogEvent("startFromJS called", nullptr);
}

struct MyTrivialTurboModulePackageProvider
    : winrt::implements<MyTrivialTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    AddTurboModuleProvider<MyTrivialTurboModule>(packageBuilder, L"MyTrivialTurboModule");
  }
};

} // namespace

TEST_CLASS (JsiSimpleTurboModuleTests) {
  TEST_METHOD(TestInstanceReload) {
    TestEventService::Initialize();

    auto reactNativeHost =
        TestReactNativeHostHolder(L"JsiSimpleTurboModuleTests", [&](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<MyTrivialTurboModulePackageProvider>());

          // See that all events are raised in JSDispatcher thread.
          host.InstanceSettings().InstanceCreated(
              [](IInspectable const & /*sender*/, InstanceCreatedEventArgs const &args) {
                TestEventService::LogEvent("Instance created event", nullptr);
                TestCheck(ReactContext(args.Context()).JSDispatcher().HasThreadAccess());
              });
          host.InstanceSettings().InstanceLoaded(
              [](IInspectable const & /*sender*/, InstanceLoadedEventArgs const &args) {
                TestEventService::LogEvent("Instance loaded event", nullptr);
                TestCheck(ReactContext(args.Context()).JSDispatcher().HasThreadAccess());
              });
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const &args) {
                TestEventService::LogEvent("Instance destroyed event", nullptr);
                TestCheck(ReactContext(args.Context()).JSDispatcher().HasThreadAccess());
              });
        });

    TestEventService::ObserveEvents({
        TestEvent{"Instance created event", nullptr},
        TestEvent{"startFromJS called", nullptr},
        TestEvent{"Instance loaded event", nullptr},
    });

    reactNativeHost.Host().ReloadInstance();

    TestEventService::ObserveEvents({
        TestEvent{"Instance destroyed event", nullptr},
        TestEvent{"Instance created event", nullptr},
        TestEvent{"startFromJS called", nullptr},
        TestEvent{"Instance loaded event", nullptr},
    });

    reactNativeHost.Host().UnloadInstance();

    TestEventService::ObserveEvents({
        TestEvent{"Instance destroyed event", nullptr},
    });
  }
};

} // namespace ReactNativeIntegrationTests
