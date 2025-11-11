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

#pragma pack(push)
#pragma warning(disable : 4100 4127 4324)
#include "codegen/msrnIntegrationTestsJSI-generated.cpp"
#include "codegen/msrnIntegrationTestsJSI.h"
#pragma pack(pop)

using namespace facebook;
using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

struct MyTrivialTurboModule : react::NativeMyTrivialTurboModuleCxxSpecJSI {
  MyTrivialTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker);

  void startFromJS(jsi::Runtime &rt) override;
};

MyTrivialTurboModule::MyTrivialTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker)
    : NativeMyTrivialTurboModuleCxxSpecJSI(std::move(jsInvoker)) {}

void MyTrivialTurboModule::startFromJS(jsi::Runtime & /*rt*/) {
  TestEventService::LogEvent("startFromJS called", nullptr);
}

struct MyTrivialTurboModulePackageProvider
    : winrt::implements<MyTrivialTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    AddTurboModuleProvider<MyTrivialTurboModule>(packageBuilder, L"MyTrivialTurboModule");
  }
};

TEST_CLASS (JsiSimpleTurboModuleTests) {
  TEST_METHOD(TestInstanceReload) {
    TestEventService::Initialize();

    std::thread::id jsThreadId{};

    auto reactNativeHost =
        TestReactNativeHostHolder(L"JsiSimpleTurboModuleTests", [&](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<MyTrivialTurboModulePackageProvider>());

          // See that all events are raised in JSDispatcher thread.
          host.InstanceSettings().InstanceCreated(
              [&](IInspectable const & /*sender*/, InstanceCreatedEventArgs const &args) {
                TestEventService::LogEvent("Instance created event", nullptr);
                // Save this thread as the js thread
                jsThreadId = std::this_thread::get_id();
              });
          host.InstanceSettings().InstanceLoaded(
              [&](IInspectable const & /*sender*/, InstanceLoadedEventArgs const &args) {
                TestEventService::LogEvent("Instance loaded event", nullptr);
                TestCheck(jsThreadId == std::this_thread::get_id());
              });
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const &args) {
                TestEventService::LogEvent("Instance destroyed event", nullptr);
                TestCheck(jsThreadId == std::this_thread::get_id());
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
