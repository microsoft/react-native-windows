// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <NativeModules.h>
#include <ReactNotificationService.h>
#include <eventWaitHandle/eventWaitHandle.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <string>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace std::string_literals;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

ReactNotificationId<int> notifyAppFromModule{L"NotificationTestModule", L"NotifyAppFromModule"};
ReactNotificationId<int> notifyModuleFromApp{L"NotificationTestModule", L"NotifyModuleFromApp"};

REACT_MODULE(NotificationTestModule)
struct NotificationTestModule {
  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;

    // Subscribe to a notification from the app.
    reactContext.Notifications().Subscribe(
        notifyModuleFromApp, [](IInspectable const & /*sender*/, ReactNotificationArgs<int> const &args) noexcept {
          TestEventService::LogEvent("NotifyModuleFromApp", args.Data());

          // Unsubscribe after the first notification.
          args.Subscription().Unsubscribe();
        });
  }

  REACT_METHOD(RunTest, L"runTest")
  void RunTest() noexcept {
    TestEventService::LogEvent("Test started", nullptr);

    // Send a notification from the module to app.
    m_reactContext.Notifications().SendNotification(notifyAppFromModule, 15);
    TestEventService::LogEvent("Test ended", nullptr);
  }

 private:
  ReactContext m_reactContext;
};

struct NotificationTestPackageProvider : winrt::implements<NotificationTestPackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    TryAddAttributedModule(packageBuilder, L"NotificationTestModule");
  }
};

} // namespace

TEST_CLASS (ReactNotificationServiceTests) {
  TEST_METHOD(Notification_Subscribe) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    bool isCalled{false};
    rns.Subscribe(fooName, nullptr, [&](IInspectable const &sender, IReactNotificationArgs const &args) noexcept {
      isCalled = true;
      TestCheck(!sender);
      TestCheck(!args.Data());
      TestCheck(!args.Subscription().Dispatcher());
      TestCheckEqual(fooName, args.Subscription().NotificationName());
      TestCheck(args.Subscription().IsSubscribed());
    });
    rns.SendNotification(fooName, nullptr, nullptr);
    TestCheck(isCalled);
  }

  TEST_METHOD(Notification_Unsubscribe) {
    IReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        fooName, nullptr, [&](IInspectable const & /*sender*/, IReactNotificationArgs const & /*args*/) noexcept {
          isCalled = true;
        });
    rns.SendNotification(fooName, nullptr, nullptr);
    TestCheck(isCalled);

    subscription.Unsubscribe();
    TestCheck(!subscription.IsSubscribed());

    isCalled = false;
    rns.SendNotification(fooName, nullptr, nullptr);
    TestCheck(!isCalled);
  }

  TEST_METHOD(Notification_UnsubscribeInHandler) {
    IReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        fooName, nullptr, [&](IInspectable const & /*sender*/, IReactNotificationArgs const &args) noexcept {
          isCalled = true;
          args.Subscription().Unsubscribe();
        });
    rns.SendNotification(fooName, nullptr, nullptr);
    TestCheck(isCalled);

    isCalled = false;
    rns.SendNotification(fooName, nullptr, nullptr);
    TestCheck(!isCalled);
  }

  TEST_METHOD(Notification_SenderAndData) {
    IReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    auto mySender = box_value(L"Hello");
    auto myData = box_value(42);
    bool isCalled{false};
    rns.Subscribe(fooName, nullptr, [&](IInspectable const &sender, IReactNotificationArgs const &args) noexcept {
      isCalled = true;
      TestCheckEqual(mySender, sender);
      TestCheckEqual(myData, args.Data());
    });
    rns.SendNotification(fooName, mySender, myData);
    TestCheck(isCalled);
  }

  TEST_METHOD(Notification_InQueue) {
    IReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    Mso::ManualResetEvent finishedEvent;
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactDispatcher dispatcher = ReactDispatcherHelper::CreateSerialDispatcher();
    bool isCalled{false};
    rns.Subscribe(
        fooName, dispatcher, [&](IInspectable const & /*sender*/, IReactNotificationArgs const &args) noexcept {
          TestCheckEqual(dispatcher, args.Subscription().Dispatcher());
          TestCheck(dispatcher.HasThreadAccess());
          isCalled = true;
          finishedEvent.Set();
        });
    rns.SendNotification(fooName, nullptr, nullptr);
    finishedEvent.Wait();
    TestCheck(isCalled);
  }

  TEST_METHOD(NotificationWrapper_Subscribe) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    bool isCalled{false};
    rns.Subscribe(
        fooNotification, [&](IInspectable const & /*sender*/, ReactNotificationArgs<void> const &args) noexcept {
          isCalled = true;
          TestCheck(!args.Subscription().Dispatcher());
          TestCheckEqual(fooNotification.NotificationName(), args.Subscription().NotificationName());
          TestCheck(args.Subscription().IsSubscribed());
        });
    rns.SendNotification(fooNotification);
    TestCheck(isCalled);
  }

  TEST_METHOD(NotificationWrapper_Unsubscribe) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        fooNotification, [&](IInspectable const & /*sender*/, ReactNotificationArgs<void> const & /*args*/) noexcept {
          isCalled = true;
        });
    rns.SendNotification(fooNotification);
    TestCheck(isCalled);

    subscription.Unsubscribe();
    TestCheck(!subscription.IsSubscribed());

    isCalled = false;
    rns.SendNotification(fooNotification);
    TestCheck(!isCalled);
  }

  TEST_METHOD(NotificationWrapper_UnsubscribeInHandler) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        fooNotification, [&](IInspectable const & /*sender*/, ReactNotificationArgs<void> const &args) noexcept {
          isCalled = true;
          args.Subscription().Unsubscribe();
        });
    rns.SendNotification(fooNotification);
    TestCheck(isCalled);

    isCalled = false;
    rns.SendNotification(fooNotification);
    TestCheck(!isCalled);
  }

  TEST_METHOD(NotificationWrapper_Sender) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    auto mySender = box_value(L"Hello");
    bool isCalled{false};
    rns.Subscribe(
        fooNotification, [&](IInspectable const &sender, ReactNotificationArgs<void> const & /*args*/) noexcept {
          isCalled = true;
          TestCheckEqual(mySender, sender);
        });
    rns.SendNotification(fooNotification, mySender);
    TestCheck(isCalled);
  }

  TEST_METHOD(NotificationWrapper_Data) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<int> fooNotification{L"Foo"};
    bool isCalled{false};
    rns.Subscribe(fooNotification, [&](IInspectable const &sender, ReactNotificationArgs<int> const &args) noexcept {
      isCalled = true;
      TestCheck(!sender);
      TestCheckEqual(42, *args.Data());
    });
    rns.SendNotification(fooNotification, 42);
    TestCheck(isCalled);
  }

  TEST_METHOD(NotificationWrapper_InQueue) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<ReactNonAbiValue<std::string>> fooNotification{L"Foo"};
    Mso::ManualResetEvent finishedEvent;
    ReactDispatcher dispatcher{ReactDispatcher::CreateSerialDispatcher()};
    bool isCalled{false};
    rns.Subscribe(
        fooNotification,
        dispatcher,
        [&](IInspectable const & /*sender*/,
            ReactNotificationArgs<ReactNonAbiValue<std::string>> const &args) noexcept {
          TestCheckEqual("Hello", *args.Data());
          TestCheckEqual(dispatcher, args.Subscription().Dispatcher());
          TestCheck(dispatcher.HasThreadAccess());
          isCalled = true;
          finishedEvent.Set();
        });
    rns.SendNotification(fooNotification, "Hello");
    finishedEvent.Wait();
    TestCheck(isCalled);
  }

  TEST_METHOD(NotificationWrapper_AutoRevoke) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        winrt::auto_revoke,
        fooNotification,
        [&](IInspectable const & /*sender*/, ReactNotificationArgs<void> const & /*args*/) noexcept {
          isCalled = true;
        });
    rns.SendNotification(fooNotification);
    TestCheck(isCalled);

    subscription = nullptr;
    TestCheck(!subscription.IsSubscribed());

    isCalled = false;
    rns.SendNotification(fooNotification);
    TestCheck(!isCalled);
  }

  TEST_METHOD(NotificationWrapper_AutoRevoke_InQueue) {
    ReactNotificationService rns{ReactNotificationServiceHelper::CreateNotificationService()};
    ReactNotificationId<void> fooNotification{L"Foo"};
    Mso::ManualResetEvent finishedEvent;
    ReactDispatcher dispatcher{ReactDispatcher::CreateSerialDispatcher()};
    ReactNotificationSubscription s;
    bool isCalled{false};
    auto subscription = rns.Subscribe(
        winrt::auto_revoke,
        fooNotification,
        dispatcher,
        [&](IInspectable const & /*sender*/, ReactNotificationArgs<void> const &args) noexcept {
          isCalled = true;
          s = args.Subscription();
          TestCheck(dispatcher.HasThreadAccess());
          finishedEvent.Set();
        });
    rns.SendNotification(fooNotification);
    finishedEvent.Wait();
    TestCheck(isCalled);

    subscription = nullptr;
    TestCheck(!s.IsSubscribed());
  }

  TEST_METHOD(NotificationBetweenAppAndModule) {
    TestEventService::Initialize();

    auto reactNativeHost =
        TestReactNativeHostHolder(L"ReactNotificationServiceTests", [](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<NotificationTestPackageProvider>());

          // Subscribe to a notification from module to app.
          ReactNotificationService::Subscribe(
              host.InstanceSettings().Notifications(),
              notifyAppFromModule,
              [](IInspectable const &, ReactNotificationArgs<int> const &args) {
                TestEventService::LogEvent("NotifyAppFromModule", args.Data());

                // Send a notification from app to the module.
                args.Subscription().NotificationService().SendNotification(notifyModuleFromApp, 42);

                // Unsubscribe after the first notification.
                args.Subscription().Unsubscribe();
              });
        });

    TestEventService::ObserveEvents({
        TestEvent{"Test started", nullptr},
        TestEvent{"NotifyAppFromModule", 15},
        TestEvent{"NotifyModuleFromApp", 42},
        TestEvent{"Test ended", nullptr},
    });
  }
};

} // namespace ReactNativeIntegrationTests
