// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  public struct ReactPropertyBag
  {
    public IReactPropertyBag Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactPropertyBag propertyBag) => propertyBag.IsValid;

    public ReactPropertyBag(IReactPropertyBag handle = null)
    {
      Handle = handle;
    }

    // Get property value by property name.
    public static T Get<T>(IReactPropertyBag handle, ReactPropertyId<T> propertyId)
    {
      if (handle == null)
      {
        return default(T);
      }
      object propertyValue = handle.Get(propertyId.Handle);
      return (T)propertyValue;
    }

    // Ensure that property is created by calling valueCreator if needed,
    // and return value by property name. The TCreateValue must return
    // either T or std::optional<T>. It must have no parameters.
    public static T GetOrCreate<T>(IReactPropertyBag handle,
      ReactPropertyId<T> propertyId,
      Func<T> createValue)
    {
      if (handle == null)
      {
        return default(T);
      }
      object propertyValue = handle.GetOrCreate(propertyId.Handle, () => createValue());
      return (T)propertyValue;
    }

    // Set property value by property name.
    public static void Set<T>(IReactPropertyBag handle, ReactPropertyId<T> propertyId, T value)
    {
      if (handle != null)
      {
        handle.Set(propertyId.Handle, value);
      }
    }

    // Removes property value by property name.
    public static void Remove<T>(IReactPropertyBag handle, ReactPropertyId<T> propertyId)
    {
      if (handle != null)
      {
        handle.Set(propertyId.Handle, null);
      }
    }

    // Get property value by property name.
    public T Get<T>(ReactPropertyId<T> propertyId)
    {
      return Get(Handle, propertyId);
    }

    // Ensure that property is created by calling valueCreator if needed,
    // and return value by property name. The TCreateValue must return
    // either T or std::optional<T>. It must have no parameters.
    public T GetOrCreate<T>(ReactPropertyId<T> propertyId, Func<T> createValue)
    {
      return GetOrCreate(Handle, propertyId, createValue);
    }

    // Set property value by property name.
    public void Set<T>(ReactPropertyId<T> propertyId, T value)
    {
      Set(Handle, propertyId, value);
    }

    // Removes property value by property name.
    public void Remove<T>(ReactPropertyId<T> propertyId)
    {
      Remove(Handle, propertyId);
    }
  }
}
