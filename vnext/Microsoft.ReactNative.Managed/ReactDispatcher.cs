// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  public struct ReactDispatcher
  {
    public bool HasThreadAccess => IsValid ? Handle.HasThreadAccess : false;

    public IReactDispatcher Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactDispatcher dispatcher) => dispatcher.IsValid;

    public ReactDispatcher(IReactDispatcher handle = null)
    {
      Handle = handle;
    }

    public void Post(ReactDispatcherCallback callback)
    {
      if (IsValid)
      {
        Handle.Post(callback);
      }
    }

    public static ReactDispatcher CreateSerialDispatcher()
    {
      return new ReactDispatcher(ReactDispatcherHelper.CreateSerialDispatcher());
    }
  }
}
