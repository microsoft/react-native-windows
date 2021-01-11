// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactNotificationService.h"
#include "ReactNotificationServiceHelper.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================
// ReactNotificationSubscription implementation
//=============================================================================

ReactNotificationSubscription::ReactNotificationSubscription(
    weak_ref<ReactNotificationService> &&notificationService,
    IReactPropertyName const &notificationName,
    IReactDispatcher const &dispatcher,
    ReactNotificationHandler const &handler) noexcept
    : m_notificationService{std::move(notificationService)},
      m_notificationName{notificationName},
      m_dispatcher{dispatcher},
      m_handler{handler} {}

ReactNotificationSubscription::~ReactNotificationSubscription() noexcept {
  Unsubscribe();
}

IReactDispatcher ReactNotificationSubscription::Dispatcher() const noexcept {
  return m_dispatcher;
}

IReactPropertyName ReactNotificationSubscription::NotificationName() const noexcept {
  return m_notificationName;
}

bool ReactNotificationSubscription::IsSubscribed() const noexcept {
  return m_isSubscribed;
}

void ReactNotificationSubscription::Unsubscribe() noexcept {
  if (m_isSubscribed.exchange(false)) {
    if (auto notificationService = m_notificationService.get()) {
      notificationService->Unsubscribe(*this);
    }
  }
}

void ReactNotificationSubscription::CallHandler(
    IInspectable const &sender,
    IReactNotificationArgs const &args) noexcept {
  if (IsSubscribed()) {
    if (m_dispatcher) {
      m_dispatcher.Post([ thisPtr = get_strong(), sender, args ]() noexcept {
        if (thisPtr->IsSubscribed()) {
          thisPtr->m_handler(sender, args);
        }
      });
    } else {
      m_handler(sender, args);
    }
  }
}

//=============================================================================
// ReactNotificationService implementation
//=============================================================================

ReactNotificationService::ReactNotificationService() = default;

ReactNotificationService::ReactNotificationService(IReactNotificationService const parentNotificationService) noexcept
    : m_parentNotificationService{parentNotificationService} {}

ReactNotificationService::~ReactNotificationService() noexcept {
  UnsubscribeAll();
}

void ReactNotificationService::ModifySubscriptions(
    IReactPropertyName const &notificationName,
    Mso::FunctorRef<SubscriptionSnapshot(SubscriptionSnapshot const &)> const &modifySnapshot) {
  // Get the current snapshot under the lock
  SubscriptionSnapshotPtr currentSnapshotPtr;
  {
    std::scoped_lock lock{m_mutex};
    auto it = m_subscriptions.find(notificationName);
    if (it != m_subscriptions.end()) {
      currentSnapshotPtr = it->second;
    }
  }

  // Modify the snapshot under the loop until we succeed.
  for (;;) {
    // Create new snapshot outside of lock.
    auto newSnapshot = modifySnapshot(currentSnapshotPtr ? *currentSnapshotPtr : SubscriptionSnapshot());

    // Try to set the new snapshot under the lock
    SubscriptionSnapshotPtr snapshotPtr;
    std::scoped_lock lock{m_mutex};
    auto it = m_subscriptions.find(notificationName);
    if (it != m_subscriptions.end()) {
      snapshotPtr = it->second;
    }

    if (currentSnapshotPtr != snapshotPtr) {
      // Some other thread already changed the snapshot for our subscription. Do it again.
      currentSnapshotPtr = std::move(snapshotPtr);
      continue;
    }

    if (newSnapshot.empty()) {
      if (it != m_subscriptions.end()) {
        m_subscriptions.erase(it);
      }
    } else {
      auto newSnapshotPtr = Mso::Make_RefCounted<SubscriptionSnapshot>(std::move(newSnapshot));
      if (it != m_subscriptions.end()) {
        it->second = std::move(newSnapshotPtr);
      } else {
        m_subscriptions.try_emplace(notificationName, std::move(newSnapshotPtr));
      }
    }

    break;
  }
}

IReactNotificationSubscription ReactNotificationService::Subscribe(
    IReactPropertyName const &notificationName,
    IReactDispatcher const &dispatcher,
    ReactNotificationHandler const &handler) noexcept {
  auto subscription = make<ReactNotificationSubscription>(get_weak(), notificationName, dispatcher, handler);
  ModifySubscriptions(
      notificationName, [&subscription](std::vector<IReactNotificationSubscription> const &snapshot) noexcept {
        std::vector<IReactNotificationSubscription> newSnapshot(snapshot);
        newSnapshot.push_back(subscription);
        return newSnapshot;
      });
  return subscription;
}

void ReactNotificationService::Unsubscribe(IReactNotificationSubscription const &subscription) noexcept {
  ModifySubscriptions(
      subscription.NotificationName(),
      [&subscription](std::vector<IReactNotificationSubscription> const &snapshot) noexcept {
        std::vector<IReactNotificationSubscription> newSnapshot(snapshot);
        auto it = std::find(newSnapshot.begin(), newSnapshot.end(), subscription);
        if (it != newSnapshot.end()) {
          newSnapshot.erase(it);
        }
        return newSnapshot;
      });
}

void ReactNotificationService::UnsubscribeAll() noexcept {
  std::map<IReactPropertyName, SubscriptionSnapshotPtr> subscriptions;
  {
    std::scoped_lock lock{m_mutex};
    subscriptions = std::move(m_subscriptions);
  }

  // Unsubscribe outside of lock.
  for (auto &namedEntry : subscriptions) {
    for (auto &subscription : *namedEntry.second) {
      subscription.Unsubscribe();
    }
  }
}

void ReactNotificationService::SendNotification(
    IReactPropertyName const &notificationName,
    IInspectable const &sender,
    IInspectable const &data) noexcept {
  SubscriptionSnapshotPtr currentSnapshotPtr;

  {
    std::scoped_lock lock{m_mutex};
    auto it = m_subscriptions.find(notificationName);
    if (it != m_subscriptions.end()) {
      currentSnapshotPtr = it->second;
    }
  }

  // Call notification handlers outside of lock.
  if (currentSnapshotPtr) {
    for (auto &subscription : *currentSnapshotPtr) {
      auto args = make<ReactNotificationArgs>(subscription, data);
      get_self<ReactNotificationSubscription>(subscription)->CallHandler(sender, args);
    }
  }

  // Call parent notification service
  if (m_parentNotificationService) {
    m_parentNotificationService.SendNotification(notificationName, sender, data);
  }
}

//=============================================================================
// ReactNotificationServiceHelper implementation
//=============================================================================

Microsoft::ReactNative::IReactNotificationService ReactNotificationServiceHelper::CreateNotificationService() noexcept {
  return make<ReactNotificationService>();
}

} // namespace winrt::Microsoft::ReactNative::implementation
