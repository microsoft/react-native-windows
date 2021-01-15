// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ReactNotificationServiceHelper.g.h"
#include <functional/functorRef.h>
#include <object/objectRefCount.h>
#include <winrt/Microsoft.ReactNative.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace std {

// Specialization to use IReactPropertyName in std::unordered_map.
template <>
struct hash<winrt::Microsoft::ReactNative::IReactPropertyName> : winrt::impl::hash_base {};

} // namespace std

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

// The ReactNotificationService manages notification subscriptions and sending messages.
// A notification service may have a parent notification service most for the automatic
// unsubscription when the notification service is deleted.
// All notifications are always sent from the root notification service that doe snot have parent service.
//
// The ReactNotificationService is optimized for sending notifications vs modifying the list of subscriptions.
// It means that it is very cheap to send messages vs adding or removing subscriptions.
// It is done through the use of subscription snapshots. The list of the subscriptions is always immutable.
// When we add or remove subscription, we create a new list of subscriptions and then atomically replace the old one.
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
  // To provide correct lifetime management, the snapshot must be ref-counted.
  using SubscriptionSnapshotPtr = Mso::RefCountedPtr<SubscriptionSnapshot>;

  void ModifySubscriptions(
      IReactPropertyName const &notificationName,
      Mso::FunctorRef<SubscriptionSnapshot(SubscriptionSnapshot const &)> const &modifySnapshot);

 private:
  const IReactNotificationService m_parentNotificationService;
  std::mutex m_mutex;
  std::unordered_map<IReactPropertyName, SubscriptionSnapshotPtr> m_subscriptions;
};

struct ReactNotificationServiceHelper {
  ReactNotificationServiceHelper() = default;

  static IReactNotificationService CreateNotificationService() noexcept;
};

struct ReactNotificationSubscription : implements<ReactNotificationSubscription, IReactNotificationSubscription> {
  ReactNotificationSubscription(
      IReactNotificationSubscription const &parentSubscription,
      weak_ref<ReactNotificationService> &&notificationService,
      IReactPropertyName const &notificationName,
      IReactDispatcher const &dispatcher) noexcept;
  ReactNotificationSubscription(
      weak_ref<ReactNotificationService> &&notificationService,
      IReactPropertyName const &notificationName,
      IReactDispatcher const &dispatcher,
      ReactNotificationHandler const &handler) noexcept;
  ~ReactNotificationSubscription() noexcept;

  IReactNotificationService NotificationService() const noexcept;
  IReactPropertyName NotificationName() const noexcept;
  IReactDispatcher Dispatcher() const noexcept;
  bool IsSubscribed() const noexcept;
  void Unsubscribe() noexcept;
  void CallHandler(IInspectable const &sender, IReactNotificationArgs const &args) noexcept;

 private:
  const IReactNotificationSubscription m_parentSubscription{nullptr};
  const weak_ref<ReactNotificationService> m_notificationService;
  const IReactPropertyName m_notificationName;
  const IReactDispatcher m_dispatcher;
  const ReactNotificationHandler m_handler;
  std::atomic_bool m_isSubscribed{true};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNotificationServiceHelper
    : ReactNotificationServiceHelperT<ReactNotificationServiceHelper, implementation::ReactNotificationServiceHelper> {
};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
