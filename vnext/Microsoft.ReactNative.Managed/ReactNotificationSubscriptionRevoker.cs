// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  public sealed class ReactNotificationSubscriptionRevoker : IDisposable
  {
    private bool isRevoked = false;

    public ReactNotificationSubscription NotificationSubscription { get; }

    public ReactDispatcher Dispatcher => NotificationSubscription.Dispatcher;

    // Name of the notification.
    public ReactPropertyName NotificationName => NotificationSubscription.NotificationName;

    public ReactNotificationService NotificationService => NotificationSubscription.NotificationService;

    // True if the subscription is still active.
    // This property is checked before notification handler is invoked.
    public bool IsSubscribed => NotificationSubscription.IsSubscribed;

    public IReactNotificationSubscription Handle => NotificationSubscription.Handle;

    public bool IsValid => NotificationSubscription.IsValid;

    public static explicit operator bool(ReactNotificationSubscriptionRevoker subscriptionRevoker) => subscriptionRevoker.IsValid;

    public ReactNotificationSubscriptionRevoker(ReactNotificationSubscription notificationSubscription)
    {
      NotificationSubscription = notificationSubscription;
    }

    public ReactNotificationSubscriptionRevoker(IReactNotificationSubscription handle = null)
      : this(new ReactNotificationSubscription(handle)) { }

    // Remove the subscription.
    // Because of the multi-threaded nature of the notifications, the handler can be still called
    // after the Unsubscribe method called if the IsSubscribed property is already checked.
    // Consider calling the Unsubscribe method and the handler in the same IReactDispatcher
    // to ensure that no handler is invoked after the Unsubscribe method call.
    public void Unsubscribe()
    {
      if (!isRevoked)
      {
        isRevoked = true;
        NotificationSubscription.Unsubscribe();
      }
    }

    ~ReactNotificationSubscriptionRevoker()
    {
      Unsubscribe();
    }

    public void Dispose()
    {
      Unsubscribe();
      GC.SuppressFinalize(this);
    }
  }
}
