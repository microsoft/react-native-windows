// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Threading;
using ReflectionMethodInfo = System.Reflection.MethodInfo;

namespace Microsoft.ReactNative.Managed
{
  static class JSValueReader
  {
    private static Lazy<Dictionary<Type, ReflectionMethodInfo>> s_readValueMethods =
        new Lazy<Dictionary<Type, ReflectionMethodInfo>>(InitReadValueMethods, LazyThreadSafetyMode.PublicationOnly);

    public static ReflectionMethodInfo GetReadValueMethod(Type valueType)
    {
      ReflectionMethodInfo readValueMethod;
      if (s_readValueMethods.Value.TryGetValue(valueType.MakeByRefType(), out readValueMethod))
      {
        return readValueMethod;
      }

      throw new ReactException($"TryReadValue extension method is not found for type {valueType.MakeByRefType().FullName}.");
    }

    private static Dictionary<Type, ReflectionMethodInfo> InitReadValueMethods()
    {
      var query = from type in typeof(JSValueReader).GetTypeInfo().Assembly.GetTypes()
                  where /*type.IsSealed && !type.IsGenericType && */!type.IsNested
                  from method in type.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic)
                  where method.IsDefined(typeof(ExtensionAttribute), false) && method.Name == "TryReadValue"
                  let parameters = method.GetParameters()
                  where parameters.Length == 2 && parameters[0].ParameterType == typeof(IJSValueReader)
                  select new { valueType = parameters[1].ParameterType, method };
      return query.ToDictionary(i => i.valueType, i => i.method);
    }

    public static bool TryReadValue(this IJSValueReader reader, out bool value)
    {
      return reader.TryGetBoolean(out value);
    }

    public static bool TryReadValue(this IJSValueReader reader, out sbyte value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (sbyte)int64Value;
      return result;
    }
    public static bool TryReadValue(this IJSValueReader reader, out short value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (short)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out int value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (int)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out long value)
    {
      return reader.TryGetInt64(out value);
    }

    public static bool TryReadValue(this IJSValueReader reader, out byte value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (byte)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out ushort value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (ushort)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out uint value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (uint)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out ulong value)
    {
      bool result = reader.TryGetInt64(out long int64Value);
      value = (ulong)int64Value;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out float value)
    {
      bool result = reader.TryGetDouble(out double doubleValue);
      value = (float)doubleValue;
      return result;
    }

    public static bool TryReadValue(this IJSValueReader reader, out double value)
    {
      return reader.TryGetDouble(out value);
    }

    public static bool TryReadValue(this IJSValueReader reader, out string value)
    {
      return reader.TryGetString(out value);
    }
  }
}
