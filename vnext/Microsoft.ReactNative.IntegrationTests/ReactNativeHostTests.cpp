// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>
#include "MockReactPackageProvider.h"
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

REACT_MODULE(TestHostModule)
struct TestHostModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const & /*reactContext*/) noexcept {
    TestEventService::LogEvent("TestHostModule::Initialize", nullptr);
  }

  REACT_FUNCTION(addValues, L"addValues", L"TestHostModuleFunctions")
  std::function<void(int a, int b)> addValues;

  REACT_METHOD(StartTests, L"startTests")
  void StartTests() noexcept {
    TestEventService::LogEvent("TestHostModule::startTests", nullptr);

    TestEventService::LogEvent("call addValues", JSValueArray{4, 7});
    addValues(4, 7);
  }

  REACT_METHOD(ReturnResult, L"returnResult")
  void ReturnResult(JSValue value) noexcept {
    TestEventService::LogEvent("TestHostModule::ReturnResult", std::move(value));
  }
};

struct TestPackageProvider : winrt::implements<TestPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    TryAddAttributedModule(packageBuilder, L"TestHostModule", true);
  }
};

} // namespace

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
    TestCheckEqual(path, host.InstanceSettings().BundleRootPath());
  }

  TEST_METHOD(Run_JSDrivenTests) {
    TestEventService::Initialize();

    auto reactNativeHost = TestReactNativeHostHolder(L"ReactNativeHostTests", [](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<TestPackageProvider>());
    });

    TestEventService::ObserveEvents(
        {TestEvent{"TestHostModule::Initialize", nullptr},
         TestEvent{"TestHostModule::startTests", nullptr},
         TestEvent{"call addValues", JSValueArray{4, 7}},
         TestEvent{"TestHostModule::ReturnResult", 11}});
  }

  TEST_METHOD(HostReloadedAsyncActionCompletedEvent) {
    TestEventService::Initialize();

    auto options = TestReactNativeHostHolder::Options{};
    options.LoadInstance = false;
    auto reactNativeHost = TestReactNativeHostHolder(
        L"ReactNativeHostTests",
        [](ReactNativeHost const &host) noexcept {
          host.ReloadInstance().Completed([](auto const &, winrt::Windows::Foundation::AsyncStatus status) mutable {
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
              TestEventService::LogEvent("InstanceLoaded::Completed", nullptr);
            } else if (status == winrt::Windows::Foundation::AsyncStatus::Canceled) {
              TestEventService::LogEvent("InstanceLoaded::Canceled", nullptr);
            } else {
              TestEventService::LogEvent("InstanceLoaded::Failed", nullptr);
            }
          });
        },
        std::move(options));

    TestEventService::ObserveEvents({TestEvent{"InstanceLoaded::Completed", nullptr}});
  }

  TEST_METHOD(LoadInstance_FiresInstanceLoaded_Success) {
    TestEventService::Initialize();

    auto options = TestReactNativeHostHolder::Options{};
    auto reactNativeHost = TestReactNativeHostHolder(L"ReactNativeHostTests", [](ReactNativeHost const &host) noexcept {
      host.InstanceSettings().InstanceLoaded(
          [](auto const &, winrt::Microsoft::ReactNative::IInstanceLoadedEventArgs args) noexcept {
            if (args.Failed()) {
              TestEventService::LogEvent("InstanceLoaded::Failed", nullptr);
            } else {
              TestEventService::LogEvent("InstanceLoaded::Success", nullptr);
            }
          });
    });

    TestEventService::ObserveEvents({TestEvent{"InstanceLoaded::Success", nullptr}});
  }

  TEST_METHOD(LoadBundleWithError_FiresInstanceLoaded_Failed) {
    TestEventService::Initialize();

    auto options = TestReactNativeHostHolder::Options{};
    auto reactNativeHost = TestReactNativeHostHolder(L"SyntaxError", [](ReactNativeHost const &host) noexcept {
      host.InstanceSettings().InstanceLoaded(
          [](auto const &, winrt::Microsoft::ReactNative::IInstanceLoadedEventArgs args) noexcept {
            if (args.Failed()) {
              TestEventService::LogEvent("InstanceLoaded::Failed", nullptr);
            } else {
              TestEventService::LogEvent("InstanceLoaded::Success", nullptr);
            }
          });
    });

    TestEventService::ObserveEvents({TestEvent{"InstanceLoaded::Failed", nullptr}});
  }

  TEST_METHOD(LoadBundleWithError_ReloadInstance_Fails) {
    TestEventService::Initialize();

    auto options = TestReactNativeHostHolder::Options{};
    options.LoadInstance = false;
    auto reactNativeHost = TestReactNativeHostHolder(
        L"SyntaxError",
        [](ReactNativeHost const &host) noexcept {
          host.ReloadInstance().Completed([](auto const &, winrt::Windows::Foundation::AsyncStatus status) mutable {
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
              TestEventService::LogEvent("InstanceLoaded::Completed", nullptr);
            } else if (status == winrt::Windows::Foundation::AsyncStatus::Canceled) {
              TestEventService::LogEvent("InstanceLoaded::Canceled", nullptr);
            } else {
              TestEventService::LogEvent("InstanceLoaded::Failed", nullptr);
            }
          });
        },
        std::move(options));

#if USE_FABRIC
    TestEventService::ObserveEvents({ TestEvent{"InstanceLoaded::Failed", nullptr} });
#else
    TestEventService::ObserveEvents({TestEvent{"InstanceLoaded::Canceled", nullptr}});
#endif
  }
};

} // namespace ReactNativeIntegrationTests
