// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactNotificationSubscription
  {
    public ReactDispatcher Dispatcher => new ReactDispatcher(IsValid ? Handle.Dispatcher : null);

    // Name of the notification.
    public ReactPropertyName NotificationName => new ReactPropertyName(IsValid ? Handle.NotificationName : null);

    public ReactNotificationService NotificationService => new ReactNotificationService(IsValid ? Handle.NotificationService : null);

    // True if the subscription is still active.
    // This property is checked before notification handler is invoked.
    public bool IsSubscribed => IsValid ? Handle.IsSubscribed : false;

    public IReactNotificationSubscription Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactNotificationSubscription subscription) => subscription.IsValid;

    public ReactNotificationSubscription(IReactNotificationSubscription handle = null)
    {
      Handle = handle;
    }

    // Remove the subscription.
    // Because of the multi-threaded nature of the notifications, the handler can be still called
    // after the Unsubscribe method called if the IsSubscribed property is already checked.
    // Consider calling the Unsubscribe method and the handler in the same IReactDispatcher
    // to ensure that no handler is invoked after the Unsubscribe method call.
    public void Unsubscribe()
    {
      if (IsValid)
      {
        Handle.Unsubscribe();
      }
    }
  }
}
