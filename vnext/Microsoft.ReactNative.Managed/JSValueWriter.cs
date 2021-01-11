// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading;

namespace Microsoft.ReactNative.Managed
{
  public delegate void WriteValueDelegate<T>(IJSValueWriter writer, T value);

  public static class JSValueWriter
  {
    private static IReadOnlyDictionary<Type, Delegate> s_writerDelegates = new Dictionary<Type, Delegate>();

    public static void WriteValue(this IJSValueWriter writer, string value)
    {
      writer.WriteString(value);
    }

    public static void WriteValue(this IJSValueWriter writer, bool value)
    {
      writer.WriteBoolean(value);
    }

    public static void WriteValue(this IJSValueWriter writer, sbyte value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, short value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, int value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, long value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, byte value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, ushort value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, uint value)
    {
      writer.WriteInt64(value);
    }

    public static void WriteValue(this IJSValueWriter writer, ulong value)
    {
      writer.WriteInt64((long)value);
    }

    public static void WriteValue(this IJSValueWriter writer, float value)
    {
      writer.WriteDouble(value);
    }

    public static void WriteValue(this IJSValueWriter writer, double value)
    {
      writer.WriteDouble(value);
    }

    public static void WriteValue(this IJSValueWriter writer, JSValue value)
    {
      value.WriteTo(writer);
    }

    public static void WriteValue(this IJSValueWriter writer, JSValueObject value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, JSValue.Void _)
    {
      writer.WriteNull();
    }

    public static void WriteValue(this IJSValueWriter writer, Dictionary<string, JSValue> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IDictionary<string, JSValue> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, ICollection<KeyValuePair<string, JSValue>> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IEnumerable<KeyValuePair<string, JSValue>> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, ReadOnlyDictionary<string, JSValue> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IReadOnlyDictionary<string, JSValue> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IReadOnlyCollection<KeyValuePair<string, JSValue>> value)
    {
      JSValue.WriteObject(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, List<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IList<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, ICollection<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IEnumerable<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, ReadOnlyCollection<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IReadOnlyList<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue(this IJSValueWriter writer, IReadOnlyCollection<JSValue> value)
    {
      JSValue.WriteArray(writer, value);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, T? value) where T : struct
    {
      if (value.HasValue)
      {
        writer.WriteValue(value.Value);
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T>(this IJSValueWriter writer, Dictionary<string, T> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IDictionary<string, T> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, ICollection<KeyValuePair<string, T>> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IEnumerable<KeyValuePair<string, T>> value)
    {
      if (value != null)
      {
        writer.WriteObjectBegin();
        foreach (var pair in value)
        {
          writer.WriteObjectProperty(pair.Key, pair.Value);
        }
        writer.WriteObjectEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T>(this IJSValueWriter writer, ReadOnlyDictionary<string, T> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IReadOnlyDictionary<string, T> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IReadOnlyCollection<KeyValuePair<string, T>> value)
    {
      writer.WriteValue(value as IEnumerable<KeyValuePair<string, T>>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, List<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IList<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, ICollection<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IEnumerable<T> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        foreach (var item in value)
        {
          writer.WriteValue(item);
        }

        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T>(this IJSValueWriter writer, ReadOnlyCollection<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IReadOnlyList<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, IReadOnlyCollection<T> value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T>(this IJSValueWriter writer, T[] value)
    {
      writer.WriteValue(value as IEnumerable<T>);
    }

    public static void WriteValue<T1>(this IJSValueWriter writer, Tuple<T1> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2>(this IJSValueWriter writer, Tuple<T1, T2> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2, T3>(this IJSValueWriter writer, Tuple<T1, T2, T3> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteValue(value.Item3);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2, T3, T4>(this IJSValueWriter writer, Tuple<T1, T2, T3, T4> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteValue(value.Item3);
        writer.WriteValue(value.Item4);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2, T3, T4, T5>(this IJSValueWriter writer, Tuple<T1, T2, T3, T4, T5> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteValue(value.Item3);
        writer.WriteValue(value.Item4);
        writer.WriteValue(value.Item5);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2, T3, T4, T5, T6>(this IJSValueWriter writer, Tuple<T1, T2, T3, T4, T5, T6> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteValue(value.Item3);
        writer.WriteValue(value.Item4);
        writer.WriteValue(value.Item5);
        writer.WriteValue(value.Item6);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T1, T2, T3, T4, T5, T6, T7>(this IJSValueWriter writer, Tuple<T1, T2, T3, T4, T5, T6, T7> value)
    {
      if (value != null)
      {
        writer.WriteArrayBegin();
        writer.WriteValue(value.Item1);
        writer.WriteValue(value.Item2);
        writer.WriteValue(value.Item3);
        writer.WriteValue(value.Item4);
        writer.WriteValue(value.Item5);
        writer.WriteValue(value.Item6);
        writer.WriteValue(value.Item7);
        writer.WriteArrayEnd();
      }
      else
      {
        writer.WriteNull();
      }
    }

    public static void WriteValue<T>(this IJSValueWriter writer, T value)
    {
      JSValueWriterOf<T>.WriteValue(writer, value);
    }

    public static void WriteObjectProperty<T>(this IJSValueWriter writer, string name, T value)
    {
      writer.WritePropertyName(name);
      writer.WriteValue(value);
    }

    public static void WriteObjectProperties<T>(this IJSValueWriter writer, T value)
    {
      var treeWriter = new JSValueTreeWriter();
      treeWriter.WriteValue(value);
      foreach (var property in treeWriter.TakeValue().AsObject())
      {
        writer.WriteObjectProperty(property.Key, property.Value);
      }
    }

    public static IJSValueWriter WriteArgs(this IJSValueWriter writer)
    {
      writer.WriteArrayBegin();
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1>(this IJSValueWriter writer, T1 arg1)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2>(this IJSValueWriter writer, T1 arg1, T2 arg2)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2, T3>(this IJSValueWriter writer, T1 arg1, T2 arg2, T3 arg3)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteValue(arg3);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2, T3, T4>(this IJSValueWriter writer,
      T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteValue(arg3);
      writer.WriteValue(arg4);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2, T3, T4, T5>(this IJSValueWriter writer,
      T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteValue(arg3);
      writer.WriteValue(arg4);
      writer.WriteValue(arg5);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2, T3, T4, T5, T6>(this IJSValueWriter writer,
      T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteValue(arg3);
      writer.WriteValue(arg4);
      writer.WriteValue(arg5);
      writer.WriteValue(arg6);
      writer.WriteArrayEnd();
      return writer;
    }

    public static IJSValueWriter WriteArgs<T1, T2, T3, T4, T5, T6, T7>(this IJSValueWriter writer,
      T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
      writer.WriteArrayBegin();
      writer.WriteValue(arg1);
      writer.WriteValue(arg2);
      writer.WriteValue(arg3);
      writer.WriteValue(arg4);
      writer.WriteValue(arg5);
      writer.WriteValue(arg6);
      writer.WriteValue(arg7);
      writer.WriteArrayEnd();
      return writer;
    }

    public static WriteValueDelegate<T> GetWriteValueDelegate<T>()
    {
      return (WriteValueDelegate<T>)GetWriteValueDelegate(typeof(T));
    }

    public static Delegate GetWriteValueDelegate(Type valueType)
    {
      // We want to generate the delegate only once even if we cannot insert it
      // to s_writerDelegates from the first attempt.
      var generatedDelegate = new Lazy<Delegate>(
        () => JSValueWriterGenerator.GenerateWriteValueDelegate(valueType));

      // We try to add the generated WriteValue delegate in a loop
      // in a lock-free thread-safe way. We do not change existing s_writerDelegates.
      // Instead we clone it, update the clone, then replace the s_writerDelegates with the updated clone.
      while (true)
      {
        var writerDelegates = s_writerDelegates; // Get the local pointer first.
        if (writerDelegates.TryGetValue(valueType, out Delegate writeDelegate))
        {
          return writeDelegate;
        }

        // The WriteValue delegate is not found. Generate it add try to add to the dictionary atomically.
        var newDelegate = generatedDelegate.Value; // Generates delegate when we are here a first time.

        // This is a quick shortcut in case if s_writerDelegates changed while we generated the value.
        // It helps to avoid cloning dictionary in a number of cases. 
        if (writerDelegates != s_writerDelegates) continue;

        // Clone the dictionary, update the clone, and try to replace the s_writerDelegates.
        var updatedWriterDelegates = new Dictionary<Type, Delegate>(writerDelegates as IDictionary<Type, Delegate>)
        {
          [valueType] = newDelegate
        };
        Interlocked.CompareExchange(ref s_writerDelegates, updatedWriterDelegates, writerDelegates);
      }
    }
  }

  static class JSValueWriterOf<T>
  {
    public static WriteValueDelegate<T> WriteValue = JSValueWriter.GetWriteValueDelegate<T>();
  }
  
  public static class JSValueWriterCodeGen<T>
  {
    public static WriteValueDelegate<T> WriteValue { get; set; }
  }
}
