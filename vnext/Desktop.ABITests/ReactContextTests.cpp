// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Microsoft.Internal.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;

namespace ABITests {

// The tests here are a development staging artifact owed to the incremental buildup of the C++/WinRT-based ABI of
// the Win32 DLL. They (or their logical equivalent) should probably get rolled into other tests once C++/WinRT-based
// instance management becomes available.
TEST_CLASS (ReactContextTests) {
 public:
  ReactContextTests() {
    IReactPropertyBag propertyBag = ReactPropertyBagHelper::CreatePropertyBag();
    IReactNotificationService notificationService = ReactNotificationServiceHelper::CreateNotificationService();
    m_context = Microsoft::Internal::TestController::CreateContext(propertyBag, notificationService);
  }

  TEST_METHOD(Properties_Get_ReturnsExpectedValue) {
    auto propertyName = ReactPropertyBagHelper::GetName(nullptr, L"Age");
    m_context.Properties().Set(propertyName, box_value(28));
    auto value = m_context.Properties().Get(propertyName);
    TestCheckEqual(28, unbox_value<int>(value));
    m_context.Properties().Set(propertyName, nullptr);
  }

  TEST_METHOD(Notifications_Send_NotificationReceived) {
    auto notificationName = ReactPropertyBagHelper::GetName(nullptr, L"wakey-wakey");
    bool received = false;
    auto subscription = m_context.Notifications().Subscribe(
        notificationName,
        nullptr,
        [&received](IInspectable const & /* sender */, IReactNotificationArgs const & /* args */) noexcept {
          received = true;
        });
    m_context.Notifications().SendNotification(notificationName, nullptr, nullptr);
    TestCheck(received);
    subscription.Unsubscribe();
  }

  // TODO: test remaining IReactContext members (CallJSFunction, EmitJSEvent ...) once instance management has been
  // hooked up

 private:
  IReactContext m_context;
};

} // namespace ABITests
