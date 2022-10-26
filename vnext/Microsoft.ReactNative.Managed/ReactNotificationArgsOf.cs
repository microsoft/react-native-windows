// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactNotificationArgs<T>
  {
    public ReactNotificationSubscription Subscription => new ReactNotificationSubscription(IsValid ? Handle.Subscription : null);

    public T Data => IsValid ? (T)Handle.Data : default(T);

    public IReactNotificationArgs Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactNotificationArgs<T> args) => args.IsValid;

    public ReactNotificationArgs(IReactNotificationArgs handle = null)
    {
      Handle = handle;
    }
  }
}
