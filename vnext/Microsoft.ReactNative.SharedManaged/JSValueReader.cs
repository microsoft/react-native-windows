// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading;
using Windows.UI.Xaml.Media;

namespace Microsoft.ReactNative.Managed
{
  delegate void ReadValueDelegate<T>(IJSValueReader reader, out T value);

  // A value can be read from IJSValueReader in one of two ways:
  // 1. There is a ReadValue extension for IJSValueReader interface that matches the type.
  // 2. We can auto-generate the read method for the type.
  static class JSValueReader
  {
    private static IReadOnlyDictionary<Type, Delegate> s_readerDelegates = new Dictionary<Type, Delegate>();

    public static void ReadValue(this IJSValueReader reader, out string value)
    {
      switch (reader.ValueType)
      {
        case JSValueType.String: value = reader.GetString(); break;
        case JSValueType.Boolean: value = reader.GetBoolean() ? "true" : "false"; break;
        case JSValueType.Int64: value = reader.GetInt64().ToString(); break;
        case JSValueType.Double: value = reader.GetDouble().ToString(); break;
        default: value = ""; break;
      }
    }

    public static void ReadValue(this IJSValueReader reader, out bool value)
    {
      switch (reader.ValueType)
      {
        case JSValueType.String: value = !string.IsNullOrEmpty(reader.GetString()); break;
        case JSValueType.Boolean: value = reader.GetBoolean(); break;
        case JSValueType.Int64: value = reader.GetInt64() != 0; break;
        case JSValueType.Double: value = reader.GetDouble() != 0; break;
        default: value = false; break;
      }
    }

    public static void ReadValue(this IJSValueReader reader, out sbyte value)
    {
      reader.ReadValue(out long val);
      value = (sbyte)val;
    }

    public static void ReadValue(this IJSValueReader reader, out short value)
    {
      reader.ReadValue(out long val);
      value = (short)val;
    }

    public static void ReadValue(this IJSValueReader reader, out int value)
    {
      reader.ReadValue(out long val);
      value = (int)val;
    }

    public static void ReadValue(this IJSValueReader reader, out long value)
    {
      switch (reader.ValueType)
      {
        case JSValueType.String:
          {
            if (!long.TryParse(reader.GetString(), out value)) { value = 0; }
            break;
          }
        case JSValueType.Boolean: value = reader.GetBoolean() ? 1 : 0; break;
        case JSValueType.Int64: value = reader.GetInt64(); break;
        case JSValueType.Double: value = (long)reader.GetDouble(); break;
        default: value = 0; break;
      }
    }

    public static void ReadValue(this IJSValueReader reader, out byte value)
    {
      reader.ReadValue(out long val);
      value = (byte)val;
    }

    public static void ReadValue(this IJSValueReader reader, out ushort value)
    {
      reader.ReadValue(out long val);
      value = (ushort)val;
    }

    public static void ReadValue(this IJSValueReader reader, out uint value)
    {
      reader.ReadValue(out long val);
      value = (uint)val;
    }

    public static void ReadValue(this IJSValueReader reader, out ulong value)
    {
      reader.ReadValue(out long val);
      value = (ulong)val;
    }

    public static void ReadValue(this IJSValueReader reader, out float value)
    {
      reader.ReadValue(out double val);
      value = (float)val;
    }

    public static void ReadValue(this IJSValueReader reader, out double value)
    {
      switch (reader.ValueType)
      {
        case JSValueType.String:
          {
            if (!double.TryParse(reader.GetString(), out value)) { value = 0; }
            break;
          }
        case JSValueType.Boolean: value = reader.GetBoolean() ? 1 : 0; break;
        case JSValueType.Int64: value = reader.GetInt64(); break;
        case JSValueType.Double: value = reader.GetDouble(); break;
        default: value = 0; break;
      }
    }

    public static void ReadValue(this IJSValueReader reader, out JSValue value)
    {
      value = JSValue.ReadFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out Brush value)
    {
      JSValue jsValue = JSValue.ReadFrom(reader);
      value = XamlHelper.BrushFrom((writer) => jsValue.WriteTo(writer));
    }

    public static void ReadValue(this IJSValueReader reader, out Dictionary<string, JSValue> value)
    {
      value = JSValue.ReadObjectFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out IDictionary<string, JSValue> value)
    {
      value = JSValue.ReadObjectFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out ICollection<KeyValuePair<string, JSValue>> value)
    {
      value = JSValue.ReadObjectFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out IEnumerable<KeyValuePair<string, JSValue>> value)
    {
      value = JSValue.ReadObjectFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out ReadOnlyDictionary<string, JSValue> value)
    {
      value = new ReadOnlyDictionary<string, JSValue>(JSValue.ReadObjectFrom(reader));
    }

    public static void ReadValue(this IJSValueReader reader, out IReadOnlyDictionary<string, JSValue> value)
    {
      value = new ReadOnlyDictionary<string, JSValue>(JSValue.ReadObjectFrom(reader));
    }

    public static void ReadValue(this IJSValueReader reader, out IReadOnlyCollection<KeyValuePair<string, JSValue>> value)
    {
      value = new ReadOnlyDictionary<string, JSValue>(JSValue.ReadObjectFrom(reader));
    }

    public static void ReadValue(this IJSValueReader reader, out List<JSValue> value)
    {
      value = JSValue.ReadArrayFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out IList<JSValue> value)
    {
      value = JSValue.ReadArrayFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out ICollection<JSValue> value)
    {
      value = JSValue.ReadArrayFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out IEnumerable<JSValue> value)
    {
      value = JSValue.ReadArrayFrom(reader);
    }

    public static void ReadValue(this IJSValueReader reader, out ReadOnlyCollection<JSValue> value)
    {
      value = new ReadOnlyCollection<JSValue>(JSValue.ReadArrayFrom(reader));
    }

    public static void ReadValue(this IJSValueReader reader, out IReadOnlyList<JSValue> value)
    {
      value = new ReadOnlyCollection<JSValue>(JSValue.ReadArrayFrom(reader));
    }

    public static void ReadValue(this IJSValueReader reader, out IReadOnlyCollection<JSValue> value)
    {
      value = new ReadOnlyCollection<JSValue>(JSValue.ReadArrayFrom(reader));
    }

    public static void ReadValue<T>(this IJSValueReader reader, out T? value) where T : struct
    {
      if (reader.ValueType != JSValueType.Null)
      {
        value = reader.ReadValue<T>();
      }
      else
      {
        value = null;
      }
    }

    public static void ReadValue<T>(this IJSValueReader reader, out Dictionary<string, T> value)
    {
      value = new Dictionary<string, T>();
      if (reader.ValueType == JSValueType.Object)
      {
        while (reader.GetNextObjectProperty(out string propertyName))
        {
          value.Add(propertyName, reader.ReadValue<T>());
        }
      }
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IDictionary<string, T> value)
    {
      reader.ReadValue(out Dictionary<string, T> dictionary);
      value = dictionary;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out ICollection<KeyValuePair<string, T>> value)
    {
      reader.ReadValue(out Dictionary<string, T> dictionary);
      value = dictionary;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IEnumerable<KeyValuePair<string, T>> value)
    {
      reader.ReadValue(out Dictionary<string, T> dictionary);
      value = dictionary;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out ReadOnlyDictionary<string, T> value)
    {
      reader.ReadValue(out Dictionary<string, T> dictionary);
      value = new ReadOnlyDictionary<string, T>(dictionary);
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IReadOnlyDictionary<string, T> value)
    {
      reader.ReadValue(out ReadOnlyDictionary<string, T> dictionary);
      value = dictionary;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IReadOnlyCollection<KeyValuePair<string, T>> value)
    {
      reader.ReadValue(out ReadOnlyDictionary<string, T> dictionary);
      value = dictionary;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out List<T> value)
    {
      value = new List<T>();
      if (reader.ValueType == JSValueType.Array)
      {
        while (reader.GetNextArrayItem())
        {
          value.Add(reader.ReadValue<T>());
        }
      }
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IList<T> value)
    {
      ReadValue(reader, out List<T> list);
      value = list;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out ICollection<T> value)
    {
      ReadValue(reader, out List<T> list);
      value = list;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IEnumerable<T> value)
    {
      ReadValue(reader, out List<T> list);
      value = list;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out ReadOnlyCollection<T> value)
    {
      ReadValue(reader, out List<T> list);
      value = new ReadOnlyCollection<T>(list);
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IReadOnlyList<T> value)
    {
      ReadValue(reader, out ReadOnlyCollection<T> collection);
      value = collection;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out IReadOnlyCollection<T> value)
    {
      ReadValue(reader, out ReadOnlyCollection<T> collection);
      value = collection;
    }

    public static void ReadValue<T>(this IJSValueReader reader, out T[] value)
    {
      ReadValue(reader, out List<T> list);
      value = list.ToArray();
    }

    public static bool SkipArrayToEnd(this IJSValueReader reader)
    {
      while (reader.GetNextArrayItem())
      {
        reader.ReadValue<JSValue>(); // Read and ignore the value
      }

      return true;
    }

    public static void ReadValue<T1>(this IJSValueReader reader, out Tuple<T1> value)
    {
      reader.ReadArgs(out T1 v1);
      value = Tuple.Create(v1);
    }

    public static void ReadValue<T1, T2>(this IJSValueReader reader, out Tuple<T1, T2> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2);
      value = Tuple.Create(v1, v2);
    }

    public static void ReadValue<T1, T2, T3>(this IJSValueReader reader, out Tuple<T1, T2, T3> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2, out T3 v3);
      value = Tuple.Create(v1, v2, v3);
    }

    public static void ReadValue<T1, T2, T3, T4>(this IJSValueReader reader, out Tuple<T1, T2, T3, T4> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2, out T3 v3, out T4 v4);
      value = Tuple.Create(v1, v2, v3, v4);
    }

    public static void ReadValue<T1, T2, T3, T4, T5>(
      this IJSValueReader reader, out Tuple<T1, T2, T3, T4, T5> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2, out T3 v3, out T4 v4, out T5 v5);
      value = Tuple.Create(v1, v2, v3, v4, v5);
    }

    public static void ReadValue<T1, T2, T3, T4, T5, T6>(
      this IJSValueReader reader, out Tuple<T1, T2, T3, T4, T5, T6> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2, out T3 v3, out T4 v4, out T5 v5, out T6 v6);
      value = Tuple.Create(v1, v2, v3, v4, v5, v6);
    }

    public static void ReadValue<T1, T2, T3, T4, T5, T6, T7>(
      this IJSValueReader reader, out Tuple<T1, T2, T3, T4, T5, T6, T7> value)
    {
      reader.ReadArgs(out T1 v1, out T2 v2, out T3 v3, out T4 v4, out T5 v5, out T6 v6, out T7 v7);
      value = Tuple.Create(v1, v2, v3, v4, v5, v6, v7);
    }

    public static bool GetNextArrayItem(this IJSValueReader reader, ref bool success)
    {
      return success = success && reader.GetNextArrayItem();
    }

    public static void ReadArgs(this IJSValueReader reader)
    {
      bool success = reader.ValueType == JSValueType.Array;
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1>(this IJSValueReader reader, out T1 arg1)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2>(this IJSValueReader reader, out T1 arg1, out T2 arg2)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2, T3>(this IJSValueReader reader, out T1 arg1, out T2 arg2, out T3 arg3)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      arg3 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T3>() : default(T3);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2, T3, T4>(this IJSValueReader reader,
      out T1 arg1, out T2 arg2, out T3 arg3, out T4 arg4)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      arg3 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T3>() : default(T3);
      arg4 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T4>() : default(T4);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2, T3, T4, T5>(this IJSValueReader reader,
      out T1 arg1, out T2 arg2, out T3 arg3, out T4 arg4, out T5 arg5)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      arg3 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T3>() : default(T3);
      arg4 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T4>() : default(T4);
      arg5 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T5>() : default(T5);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2, T3, T4, T5, T6>(this IJSValueReader reader,
      out T1 arg1, out T2 arg2, out T3 arg3, out T4 arg4, out T5 arg5, out T6 arg6)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      arg3 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T3>() : default(T3);
      arg4 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T4>() : default(T4);
      arg5 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T5>() : default(T5);
      arg6 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T6>() : default(T6);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadArgs<T1, T2, T3, T4, T5, T6, T7>(this IJSValueReader reader,
      out T1 arg1, out T2 arg2, out T3 arg3, out T4 arg4, out T5 arg5, out T6 arg6, out T7 arg7)
    {
      bool success = reader.ValueType == JSValueType.Array;
      arg1 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T1>() : default(T1);
      arg2 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T2>() : default(T2);
      arg3 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T3>() : default(T3);
      arg4 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T4>() : default(T4);
      arg5 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T5>() : default(T5);
      arg6 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T6>() : default(T6);
      arg7 = reader.GetNextArrayItem(ref success) ? reader.ReadValue<T7>() : default(T7);
      _ = success && reader.SkipArrayToEnd();
    }

    public static void ReadValue<T>(this IJSValueReader reader, out T value)
    {
      JSValueReaderOf<T>.ReadValue(reader, out value);
    }

    public static T ReadValue<T>(this IJSValueReader reader)
    {
      JSValueReaderOf<T>.ReadValue(reader, out T value);
      return value;
    }

    public static void ReadValue<T>(this JSValue jsValue, out T value)
    {
      JSValueReaderOf<T>.ReadValue(new JSValueTreeReader(jsValue), out value);
    }

    public static T ReadValue<T>(this JSValue jsValue)
    {
      JSValueReaderOf<T>.ReadValue(new JSValueTreeReader(jsValue), out T value);
      return value;
    }

    public static ReadValueDelegate<T> GetReadValueDelegate<T>()
    {
      return (ReadValueDelegate<T>)GetReadValueDelegate(typeof(T));
    }

    public static Delegate GetReadValueDelegate(Type valueType)
    {
      // We want to generate the delegate only once even if we cannot insert it
      // to s_readerDelegates from the first attempt.
      var generatedDelegate = new Lazy<Delegate>(
        () => JSValueReaderGenerator.GenerateReadValueDelegate(valueType));

      // We try to add the generated ReadValue delegate in a loop
      // in a lock-free thread-safe way. We do not change existing s_readerDelegates.
      // Instead we clone it, update the clone, then replace the s_readerDelegates with the updated clone.
      while (true)
      {
        var readerDelegates = s_readerDelegates; // Get the local pointer first.
        if (readerDelegates.TryGetValue(valueType, out Delegate readDelegate))
        {
          return readDelegate;
        }

        // The ReadValue delegate is not found. Generate it add try to add to the dictionary atomically.
        var newDelegate = generatedDelegate.Value; // Generates delegate when we are here a first time.

        // This is a quick shortcut in case if s_readerDelegates changed while we generated the value.
        // It helps to avoid cloning dictionary in a number of cases. 
        if (readerDelegates != s_readerDelegates) continue;

        // Clone the dictionary, update the clone, and try to replace the s_readerDelegates.
        var updatedReaderDelegates = new Dictionary<Type, Delegate>(readerDelegates as IDictionary<Type, Delegate>)
        {
          [valueType] = newDelegate
        };
        Interlocked.CompareExchange(ref s_readerDelegates, updatedReaderDelegates, readerDelegates);
      }
    }
  }

  // This class provides constant time access to the ReadValue delegate.
  static class JSValueReaderOf<T>
  {
    public static ReadValueDelegate<T> ReadValue = JSValueReader.GetReadValueDelegate<T>();
  }
}
