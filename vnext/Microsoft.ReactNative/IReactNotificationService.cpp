// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactNotificationService.h"
#include "ReactNotificationServiceHelper.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================
// IReactNotificationSubscription implementation
//=============================================================================

// Common interface to share functionality between ReactNotificationSubscription and ReactNotificationSubscriptionView
MSO_GUID(IReactNotificationSubscriptionPrivate, "09437980-3508-4690-930c-7c310e205e6b")
struct IReactNotificationSubscriptionPrivate : ::IUnknown {
  virtual void SetParent(IReactNotificationSubscription const &parentSubscription) noexcept = 0;
  virtual void CallHandler(
      Windows::Foundation::IInspectable const &sender,
      Windows::Foundation::IInspectable const &data) noexcept = 0;
};

// The Notification subscription class.
// Instances of this class are stored in the "child" notification services.
struct ReactNotificationSubscription
    : implements<ReactNotificationSubscription, IReactNotificationSubscription, IReactNotificationSubscriptionPrivate> {
  ReactNotificationSubscription(
      Mso::RefCountedPtr<std::mutex> const &mutex,
      weak_ref<ReactNotificationService> &&notificationService,
      IReactPropertyName const &notificationName,
      IReactDispatcher const &dispatcher,
      ReactNotificationHandler const &handler) noexcept
      : m_mutex{mutex},
        m_notificationService{std::move(notificationService)},
        m_notificationName{notificationName},
        m_dispatcher{dispatcher},
        m_handler{handler} {}

  ~ReactNotificationSubscription() noexcept {
    Unsubscribe();
  }

 public: // IReactNotificationSubscription implementation
  IReactNotificationService NotificationService() const noexcept {
    return m_notificationService.get().as<IReactNotificationService>();
  }

  IReactPropertyName NotificationName() const noexcept {
    return m_notificationName;
  }

  IReactDispatcher Dispatcher() const noexcept {
    return m_dispatcher;
  }

  bool IsSubscribed() const noexcept {
    return GetHandler() != nullptr;
  }

  void Unsubscribe() noexcept {
    if (m_parentSubscription) {
      m_parentSubscription.Unsubscribe();
    }

    bool isSubscribed{false};
    {
      std::scoped_lock lock{*m_mutex};
      if (m_handler) {
        isSubscribed = true;
        // Remove handler to free any objects captured by it.
        m_handler = nullptr;
      }
    }

    if (isSubscribed) {
      if (auto notificationService = m_notificationService.get()) {
        notificationService->Unsubscribe(*this);
      }
    }
  }

 public: // IReactNotificationSubscriptionPrivate implementation
  void SetParent(IReactNotificationSubscription const &parentSubscription) noexcept override {
    m_parentSubscription = parentSubscription;
  }

  void CallHandler(IInspectable const &sender, IInspectable const &data) noexcept override {
    auto args = make<ReactNotificationArgs>(*this, data);
    if (auto handler = GetHandler()) {
      if (m_dispatcher) {
        m_dispatcher.Post([thisPtr = get_strong(), sender, args]() noexcept {
          if (auto handler = thisPtr->GetHandler()) {
            handler(sender, args);
          }
        });
      } else {
        handler(sender, args);
      }
    }
  }

 private:
  ReactNotificationHandler GetHandler() const noexcept {
    std::scoped_lock lock{*m_mutex};
    return m_handler;
  }

 private:
  Mso::RefCountedPtr<std::mutex> m_mutex;
  IReactNotificationSubscription m_parentSubscription{nullptr};
  const weak_ref<ReactNotificationService> m_notificationService{nullptr};
  const IReactPropertyName m_notificationName{nullptr};
  const IReactDispatcher m_dispatcher{nullptr};
  ReactNotificationHandler m_handler{nullptr};
};

// The notification subscription view to wrap up child notification service.
// Instances of this class are stored in the parent notification services.
struct ReactNotificationSubscriptionView : implements<
                                               ReactNotificationSubscriptionView,
                                               IReactNotificationSubscription,
                                               IReactNotificationSubscriptionPrivate> {
  ReactNotificationSubscriptionView(
      weak_ref<ReactNotificationService> &&notificationService,
      IReactNotificationSubscription const &childSubscription) noexcept
      : m_notificationService{std::move(notificationService)}, m_childSubscription{weak_ref(childSubscription)} {
    childSubscription.as<IReactNotificationSubscriptionPrivate>()->SetParent(*this);
  }

  ~ReactNotificationSubscriptionView() noexcept {
    Unsubscribe();
  }

 public: // IReactNotificationSubscription implementation
  IReactNotificationService NotificationService() const noexcept {
    return m_notificationService.get().as<IReactNotificationService>();
  }

  IReactPropertyName NotificationName() const noexcept {
    if (auto childSubscription = m_childSubscription.get()) {
      return childSubscription.NotificationName();
    } else {
      return IReactPropertyName{nullptr};
    }
  }

  IReactDispatcher Dispatcher() const noexcept {
    if (auto childSubscription = m_childSubscription.get()) {
      return childSubscription.Dispatcher();
    } else {
      return IReactDispatcher{nullptr};
    }
  }

  bool IsSubscribed() const noexcept {
    return m_isSubscribed;
  }

  void Unsubscribe() noexcept {
    if (m_parentSubscription) {
      m_parentSubscription.Unsubscribe();
    }

    if (m_isSubscribed.exchange(false)) {
      if (auto notificationService = m_notificationService.get()) {
        notificationService->Unsubscribe(*this);
      }
    }
  }

 public: // IReactNotificationSubscriptionPrivate implementation
  void SetParent(IReactNotificationSubscription const &parentSubscription) noexcept override {
    m_parentSubscription = parentSubscription;
  }

  void CallHandler(IInspectable const &sender, IInspectable const &data) noexcept override {
    if (auto childSubscription = m_childSubscription.get()) {
      childSubscription.as<IReactNotificationSubscriptionPrivate>()->CallHandler(sender, data);
    }
  }

 private:
  IReactNotificationSubscription m_parentSubscription{nullptr};
  const weak_ref<IReactNotificationSubscription> m_childSubscription{nullptr};
  const weak_ref<ReactNotificationService> m_notificationService{nullptr};
  std::atomic_bool m_isSubscribed{true};
};

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
    std::scoped_lock lock{*m_mutex};
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
    std::scoped_lock lock{*m_mutex};
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
  VerifyElseCrashSz(notificationName, "notificationName must be not null");
  VerifyElseCrashSz(handler, "handler must be not null");

  IReactNotificationSubscription subscription =
      make<ReactNotificationSubscription>(m_mutex, get_weak(), notificationName, dispatcher, handler);
  AddSubscription(notificationName, subscription);
  AddSubscriptionToParent(notificationName, subscription);
  return subscription;
}

void ReactNotificationService::AddSubscription(
    IReactPropertyName const &notificationName,
    IReactNotificationSubscription const &subscription) noexcept {
  ModifySubscriptions(
      notificationName, [&subscription](std::vector<IReactNotificationSubscription> const &snapshot) noexcept {
        auto newSnapshot = std::vector<IReactNotificationSubscription>(snapshot);
        newSnapshot.push_back(subscription);
        return newSnapshot;
      });
}

void ReactNotificationService::AddSubscriptionToParent(
    IReactPropertyName const &notificationName,
    IReactNotificationSubscription const &subscription) noexcept {
  if (m_parentNotificationService) {
    get_self<ReactNotificationService>(m_parentNotificationService)
        ->AddSubscriptionFromChild(notificationName, subscription);
  }
}

void ReactNotificationService::AddSubscriptionFromChild(
    IReactPropertyName const &notificationName,
    IReactNotificationSubscription const &childSubscription) noexcept {
  auto subscription = make<ReactNotificationSubscriptionView>(get_weak(), childSubscription);
  AddSubscription(notificationName, subscription);
  AddSubscriptionToParent(notificationName, subscription);
}

void ReactNotificationService::Unsubscribe(IReactNotificationSubscription const &subscription) noexcept {
  // The subscription will call the parent Unsubscribe on its own.
  ModifySubscriptions(
      subscription.NotificationName(),
      [&subscription](std::vector<IReactNotificationSubscription> const &snapshot) noexcept {
        auto newSnapshot = std::vector<IReactNotificationSubscription>(snapshot);
        auto it = std::find(newSnapshot.begin(), newSnapshot.end(), subscription);
        if (it != newSnapshot.end()) {
          newSnapshot.erase(it);
        }
        return newSnapshot;
      });
}

void ReactNotificationService::UnsubscribeAll() noexcept {
  // The subscription will call the parent Unsubscribe on its own.
  decltype(m_subscriptions) subscriptions;
  {
    std::scoped_lock lock{*m_mutex};
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
  if (m_parentNotificationService) {
    // Notification are always sent from the root notification service.
    m_parentNotificationService.SendNotification(notificationName, sender, data);
  } else {
    SubscriptionSnapshotPtr currentSnapshotPtr;

    {
      std::scoped_lock lock{*m_mutex};
      auto it = m_subscriptions.find(notificationName);
      if (it != m_subscriptions.end()) {
        currentSnapshotPtr = it->second;
      }
    }

    // Call notification handlers outside of lock.
    if (currentSnapshotPtr) {
      for (auto &subscription : *currentSnapshotPtr) {
        subscription.as<IReactNotificationSubscriptionPrivate>()->CallHandler(sender, data);
      }
    }
  }
}

//=============================================================================
// ReactNotificationServiceHelper implementation
//=============================================================================

Microsoft::ReactNative::IReactNotificationService ReactNotificationServiceHelper::CreateNotificationService() noexcept {
  return make<ReactNotificationService>();
}

} // namespace winrt::Microsoft::ReactNative::implementation
