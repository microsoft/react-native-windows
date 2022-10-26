// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  public struct ReactNotificationId<T>
  {
    public ReactPropertyName NotificationName { get; }

    public ReactPropertyNamespace Namespace => NotificationName.Namespace;

    public string NamespaceName => NotificationName.NamespaceName;

    public string LocalName => NotificationName.LocalName;

    public IReactPropertyName Handle => NotificationName.Handle;

    public bool IsValid => NotificationName.IsValid;

    public static explicit operator bool(ReactNotificationId<T> notificationId) => notificationId.IsValid;

    public ReactNotificationId(ReactPropertyName propertyName)
    {
      NotificationName = propertyName;
    }

    public ReactNotificationId(IReactPropertyName handle = null)
      : this(new ReactPropertyName(handle)) { }

    public ReactNotificationId(string localName)
      : this(new ReactPropertyName(localName)) { }

    public ReactNotificationId(string namespaceName, string localName)
      : this(new ReactPropertyName(namespaceName, localName)) { }

    public ReactNotificationId(ReactPropertyNamespace ns, string localName)
      : this(new ReactPropertyName(ns, localName)) { }

    public ReactNotificationId(IReactPropertyNamespace ns, string localName)
      : this(new ReactPropertyName(ns, localName)) { }
  }
}
