// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;

namespace Microsoft.ReactNative.Managed
{
  internal static class ReflectionHelpers
  {
    public static CustomAttributeData GetCustomAttributeData<T>(MemberInfo memberInfo) where T : Attribute
    {
      foreach (var customAttributeData in memberInfo.CustomAttributes)
      {
        if (customAttributeData.AttributeType == typeof(T))
        {
          return customAttributeData;
        }
      }
      return null;
    }

    public static string GetStringAttributeValueByName(CustomAttributeData customAttributeData, string attributeName)
    {
      foreach (var args in customAttributeData.NamedArguments)
      {
        if (args.MemberName == attributeName && args.TypedValue.ArgumentType == typeof(string))
        {
          return args.TypedValue.Value as string;
        }
      }
      return null;
    }

    public static T? GetAttributeValueByName<T>(CustomAttributeData customAttributeData, string attributeName) where T : struct
    {
      foreach (var args in customAttributeData.NamedArguments)
      {
        if (args.MemberName == attributeName && args.TypedValue.ArgumentType == typeof(T))
        {
          return (T)args.TypedValue.Value;
        }
      }
      return null;
    }
  }
}
