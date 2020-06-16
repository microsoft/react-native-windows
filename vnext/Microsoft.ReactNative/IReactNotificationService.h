// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ReactNotificationServiceHelper.g.h"
#include <functional/functorRef.h>
#include <object/objectRefCount.h>
#include <winrt/Microsoft.ReactNative.h>
#include <atomic>
#include <map>
#include <mutex>
#include <vector>

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactNotificationArgs : implements<ReactNotificationArgs, IReactNotificationArgs> {
  ReactNotificationArgs(IReactNotificationSubscription const &subscription, IInspectable const &data) noexcept
      : m_subscription{subscription}, m_data{data} {}

  IReactNotificationSubscription Subscription() const noexcept {
    return m_subscription;
  }

  IInspectable Data() const noexcept {
    return m_data;
  }

 private:
  IReactNotificationSubscription m_subscription;
  IInspectable m_data;
};

struct ReactNotificationService : implements<ReactNotificationService, IReactNotificationService> {
  ReactNotificationService();
  explicit ReactNotificationService(IReactNotificationService const parentNotificationService) noexcept;
  ~ReactNotificationService() noexcept;

  IReactNotificationSubscription Subscribe(
      IReactPropertyName const &notificationName,
      IReactDispatcher const &dispatcher,
      ReactNotificationHandler const &handler) noexcept;

  void Unsubscribe(IReactNotificationSubscription const &subscription) noexcept;

  void UnsubscribeAll() noexcept;

  void SendNotification(
      IReactPropertyName const &notificationName,
      IInspectable const &sender,
      IInspectable const &data) noexcept;

 private:
  // We treat subscription snapshots as immutable data.
  using SubscriptionSnapshot = std::vector<IReactNotificationSubscription>;
  using SubscriptionSnapshotPtr = Mso::RefCountedPtr<SubscriptionSnapshot>;

  void ModifySubscriptions(
      IReactPropertyName const &notificationName,
      Mso::FunctorRef<SubscriptionSnapshot(SubscriptionSnapshot const &)> const &modifySnapshot);

 private:
  std::mutex m_mutex;
  std::map<IReactPropertyName, SubscriptionSnapshotPtr> m_subscriptions;
  IReactNotificationService m_parentNotificationService;
};

struct ReactNotificationServiceHelper {
  ReactNotificationServiceHelper() = default;

  static IReactNotificationService CreateNotificationService() noexcept;
};

struct ReactNotificationSubscription : implements<ReactNotificationSubscription, IReactNotificationSubscription> {
  ReactNotificationSubscription(
      weak_ref<ReactNotificationService> &&notificationService,
      IReactPropertyName const &notificationName,
      IReactDispatcher const &dispatcher,
      ReactNotificationHandler const &handler) noexcept;
  ~ReactNotificationSubscription() noexcept;

  IReactPropertyName NotificationName() const noexcept;
  IReactDispatcher Dispatcher() const noexcept;
  bool IsSubscribed() const noexcept;
  void Unsubscribe() noexcept;
  void CallHandler(IInspectable const &sender, IReactNotificationArgs const &args) noexcept;

 private:
  weak_ref<ReactNotificationService> m_notificationService;
  IReactPropertyName m_notificationName;
  IReactDispatcher m_dispatcher;
  ReactNotificationHandler m_handler;
  std::atomic_bool m_isSubscribed{true};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNotificationServiceHelper
    : ReactNotificationServiceHelperT<ReactNotificationServiceHelper, implementation::ReactNotificationServiceHelper> {
};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
