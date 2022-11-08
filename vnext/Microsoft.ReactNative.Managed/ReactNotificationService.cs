// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactNotificationService
  {
    public IReactNotificationService Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactNotificationService service) => service.IsValid;

    public ReactNotificationService(IReactNotificationService handle = null)
    {
      Handle = handle;
    }

    public static ReactNotificationSubscriptionRevoker SubscribeWithAutoRevoke<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      ReactDispatcher dispatcher,
      ReactNotificationHandler<T> handler)
    {
      return new ReactNotificationSubscriptionRevoker(Subscribe<T>(handle, notificationId, dispatcher, handler));
    }

    public static ReactNotificationSubscriptionRevoker SubscribeWithAutoRevoke<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      ReactNotificationHandler<T> handler)
    {
      return new ReactNotificationSubscriptionRevoker(Subscribe<T>(handle, notificationId, new ReactDispatcher(null), handler));
    }

    public ReactNotificationSubscriptionRevoker SubscribeWithAutoRevoke<T>(
      ReactNotificationId<T> notificationId,
      ReactDispatcher dispatcher,
      ReactNotificationHandler<T> handler)
    {
      return SubscribeWithAutoRevoke(Handle, notificationId, dispatcher, handler);
    }

    public ReactNotificationSubscriptionRevoker SubscribeWithAutoRevoke<T>(
      ReactNotificationId<T> notificationId,
      ReactNotificationHandler<T> handler)
    {
      return SubscribeWithAutoRevoke(Handle, notificationId, handler);
    }

    public static ReactNotificationSubscription Subscribe<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      ReactDispatcher dispatcher,
      ReactNotificationHandler<T> handler)
    {
      IReactNotificationSubscription subscriptionHandle = handle?.Subscribe(
        notificationId.Handle,
        dispatcher.Handle,
        (sender, args) => handler(sender, new ReactNotificationArgs<T>(args)));
      return new ReactNotificationSubscription(subscriptionHandle);
    }

    public static ReactNotificationSubscription Subscribe<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      ReactNotificationHandler<T> handler)
    {
      return Subscribe<T>(handle, notificationId, new ReactDispatcher(null), handler);
    }

    public ReactNotificationSubscription Subscribe<T>(
      ReactNotificationId<T> notificationId,
      ReactDispatcher dispatcher,
      ReactNotificationHandler<T> handler)
    {
      return Subscribe(Handle, notificationId, dispatcher, handler);
    }

    public ReactNotificationSubscription Subscribe<T>(
      ReactNotificationId<T> notificationId,
      ReactNotificationHandler<T> handler)
    {
      return Subscribe(Handle, notificationId, handler);
    }

    public static void SendNotification<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      object sender,
      T value)
    {
      handle?.SendNotification(notificationId.Handle, sender, value);
    }

    public static void SendNotification<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      object sender)
    {
      handle?.SendNotification(notificationId.Handle, sender, null);
    }

    public static void SendNotification<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId,
      T value)
    {
      handle?.SendNotification(notificationId.Handle, null, value);
    }

    public static void SendNotification<T>(
      IReactNotificationService handle,
      ReactNotificationId<T> notificationId)
    {
      handle?.SendNotification(notificationId.Handle, null, null);
    }

    public void SendNotification<T>(
      ReactNotificationId<T> notificationId,
      object sender,
      T value)
    {
      SendNotification(Handle, notificationId, sender, value);
    }

    public void SendNotification<T>(
      ReactNotificationId<T> notificationId,
      object sender)
    {
      SendNotification(Handle, notificationId, sender);
    }

    public void SendNotification<T>(
      ReactNotificationId<T> notificationId,
      T value)
    {
      SendNotification(Handle, notificationId, value);
    }

    public void SendNotification<T>(ReactNotificationId<T> notificationId)
    {
      SendNotification(Handle, notificationId);
    }
  }
}
