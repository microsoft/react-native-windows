// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Threading;
using ReflectionMethodInfo = System.Reflection.MethodInfo;

namespace Microsoft.ReactNative.Managed
{
  delegate void ValueWriter(IJSValueWriter writer, object value);

  static class JSValueWriter
  {
    private static Lazy<Dictionary<Type, ReflectionMethodInfo>> s_writeValueMethods =
        new Lazy<Dictionary<Type, ReflectionMethodInfo>>(InitWriteValueMethods, LazyThreadSafetyMode.PublicationOnly);

    public static ReflectionMethodInfo GetWriteValueMethod(Type valueType)
    {
      ReflectionMethodInfo writeValueMethod;
      if (s_writeValueMethods.Value.TryGetValue(valueType, out writeValueMethod))
      {
        return writeValueMethod;
      }

      throw new ReactException($"WriteValue extension method is not found for type {valueType.FullName}.");
    }

    private static Dictionary<Type, ReflectionMethodInfo> InitWriteValueMethods()
    {
      var query = from type in typeof(JSValueWriter).GetTypeInfo().Assembly.GetTypes()
                  where /*type.IsSealed && !type.IsGenericType && */!type.IsNested
                  from method in type.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic)
                  where method.IsDefined(typeof(ExtensionAttribute), false) && method.Name == "WriteValue"
                  let parameters = method.GetParameters()
                  where parameters.Length == 2 && parameters[0].ParameterType == typeof(IJSValueWriter)
                  select new { valueType = parameters[1].ParameterType, method };
      return query.ToDictionary(i => i.valueType, i => i.method);
    }

    public static bool WriteValue(this IJSValueWriter writer, bool value)
    {
      return writer.WriteBoolean(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, sbyte value)
    {
      return writer.WriteInt64(value);
    }
    public static bool WriteValue(this IJSValueWriter writer, short value)
    {
      return writer.WriteInt64(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, int value)
    {
      return writer.WriteInt64(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, long value)
    {
      return writer.WriteInt64(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, byte value)
    {
      return writer.WriteInt64(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, ushort value)
    {
      return writer.WriteInt64(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, uint value)
    {
      return writer.WriteInt64((int)value);
    }

    public static bool WriteValue(this IJSValueWriter writer, ulong value)
    {
      return writer.WriteInt64((long)value);
    }

    public static bool WriteValue(this IJSValueWriter writer, float value)
    {
      return writer.WriteDouble(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, double value)
    {
      return writer.WriteDouble(value);
    }

    public static bool WriteValue(this IJSValueWriter writer, string value)
    {
      return writer.WriteString(value);
    }

    public static bool WriteProperty<T>(this IJSValueWriter writer, string name, T value)
    {
      return writer.WritePropertyName(name) && writer.WriteValueObject(value);
    }

    public static void WriteArgs(this IJSValueWriter writer, params object[] args)
    {
      writer.WriteArrayBegin();
      foreach (var arg in args)
      {
        writer.WriteValueObject(arg);
      }
      writer.WriteArrayEnd();
    }

    public static bool WriteValueObject(this IJSValueWriter writer, object value)
    {
      ReflectionMethodInfo writeValue = GetWriteValueMethod(value.GetType());
      return (bool)writeValue.Invoke(null, new object[] { writer, value });
    }
  }
}
